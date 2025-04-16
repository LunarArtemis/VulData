YR_API YR_MEMORY_BLOCK* yr_process_get_first_memory_block(
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) iterator->context;
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  context->current_block.size = 0;
  context->current_block.base = (size_t)
                                    proc_info->si.lpMinimumApplicationAddress;

  return yr_process_get_next_memory_block(iterator);
}