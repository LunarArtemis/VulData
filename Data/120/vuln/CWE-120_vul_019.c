YR_API YR_MEMORY_BLOCK* yr_process_get_next_memory_block(
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) iterator->context;
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  char buf[4096];

  proc_info->vm_entry.pve_path = buf;
  proc_info->vm_entry.pve_pathlen = sizeof(buf);

  iterator->last_error = ERROR_SUCCESS;

  uint64_t current_begin = context->current_block.base +
                           context->current_block.size;

  uint64_t max_process_memory_chunk;

  yr_get_configuration(
      YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK, (void*) &max_process_memory_chunk);

  if (proc_info->vm_entry.pve_end <= current_begin)
  {
    if (ptrace(
            PT_VM_ENTRY, proc_info->pid, (char*) (&proc_info->vm_entry), 0) ==
        -1)
    {
      return NULL;
    }
    else
    {
      current_begin = proc_info->vm_entry.pve_start;
    }
  }

  context->current_block.base = current_begin;
  context->current_block.size = yr_min(
      proc_info->vm_entry.pve_end - current_begin + 1,
      max_process_memory_chunk);

  assert(context->current_block.size > 0);

  return &context->current_block;
}