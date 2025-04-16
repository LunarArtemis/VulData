YR_API YR_MEMORY_BLOCK* yr_process_get_next_memory_block(
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) iterator->context;
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  char buffer[PATH_MAX];
  char perm[5];
  uint64_t begin, end;

  uint64_t current_begin = context->current_block.base +
                           context->current_block.size;

  uint64_t max_process_memory_chunk;

  yr_get_configuration(
      YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK, (void*) &max_process_memory_chunk);

  if (proc_info->next_block_end <= current_begin)
  {
    int n, path_start;

    while (fgets(buffer, sizeof(buffer), proc_info->maps) != NULL)
    {
      // If we haven't read the whole line, skip over the rest.
      if (strrchr(buffer, '\n') == NULL)
      {
        int c;
        do
        {
          c = fgetc(proc_info->maps);
        } while (c >= 0 && c != '\n');
      }

      // Each row in /proc/$PID/maps describes a region of contiguous virtual
      // memory in a process or thread. Each row has the following fields:
      //
      // address           perms offset  dev   inode   pathname
      // 08048000-08056000 r-xp 00000000 03:0c 64593   /usr/sbin/gpm
      //
      n = sscanf(
          buffer,
          "%" SCNx64 "-%" SCNx64 " %4s "
          "%" SCNx64 " %" SCNx64 ":%" SCNx64 " %" SCNu64 " %n",
          &begin,
          &end,
          perm,
          &(proc_info->map_offset),
          &(proc_info->map_dmaj),
          &(proc_info->map_dmin),
          &(proc_info->map_ino),
          &path_start);

      // If the row was parsed correctly sscan must return 7.
      if (n == 7)
      {
        // path_start contains the offset within buffer where the path starts,
        // the path should start with /.
        if (buffer[path_start] == '/')
          strncpy(
              proc_info->map_path,
              buffer + path_start,
              sizeof(proc_info->map_path) - 1);
        else
          proc_info->map_path[0] = '\0';
        break;
      }
    }

    if (n == 7)
    {
      current_begin = begin;
      proc_info->next_block_end = end;
    }
    else
    {
      YR_DEBUG_FPRINTF(2, stderr, "+ %s() = NULL\n", __FUNCTION__);
      return NULL;
    }
  }

  context->current_block.base = current_begin;
  context->current_block.size = yr_min(
      proc_info->next_block_end - current_begin, max_process_memory_chunk);

  assert(context->current_block.size > 0);

  iterator->last_error = ERROR_SUCCESS;

  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "- %s() {} = %p // .base=0x%" PRIx64 " .size=%" PRIu64 "\n",
      __FUNCTION__,
      result,
      context->current_block.base,
      context->current_block.size);

  return &context->current_block;
}