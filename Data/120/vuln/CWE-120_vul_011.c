YR_API YR_MEMORY_BLOCK* yr_process_get_next_memory_block(
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) iterator->context;
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  MEMORY_BASIC_INFORMATION mbi;
  void* address =
      (void*) (context->current_block.base + context->current_block.size);
  uint64_t max_process_memory_chunk;

  yr_get_configuration(
      YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK, (void*) &max_process_memory_chunk);

  iterator->last_error = ERROR_SUCCESS;

  while (address < proc_info->si.lpMaximumApplicationAddress &&
         VirtualQueryEx(proc_info->hProcess, address, &mbi, sizeof(mbi)) != 0)
  {
    // mbi.RegionSize can overflow address while scanning a 64-bit process
    // with a 32-bit YARA.
    if ((uint8_t*) address + mbi.RegionSize <= (uint8_t*) address)
      break;

    if (mbi.State == MEM_COMMIT && ((mbi.Protect & PAGE_NOACCESS) == 0))
    {
      size_t chuck_size =
          mbi.RegionSize -
          (size_t) (((uint8_t*) address) - ((uint8_t*) mbi.BaseAddress));

      if (((uint64_t) chuck_size) > max_process_memory_chunk)
      {
        chuck_size = (size_t) max_process_memory_chunk;
      }

      context->current_block.base = (size_t) address;
      context->current_block.size = chuck_size;

      return &context->current_block;
    }

    address = (uint8_t*) mbi.BaseAddress + mbi.RegionSize;
  }

  return NULL;
}