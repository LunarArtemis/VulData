YR_API YR_MEMORY_BLOCK* yr_process_get_first_memory_block(
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) iterator->context;

  context->current_block.base = 0;
  context->current_block.size = 0;

  return yr_process_get_next_memory_block(iterator);
}