YR_API YR_MEMORY_BLOCK* yr_process_get_first_memory_block(
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_DEBUG_FPRINTF(2, stderr, "+ %s() {\n", __FUNCTION__);

  YR_MEMORY_BLOCK* result = NULL;
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) iterator->context;
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  if (fseek(proc_info->maps, 0, SEEK_SET) != 0)
  {
    result = NULL;
    goto _exit;
  }

  proc_info->next_block_end = 0;

  result = yr_process_get_next_memory_block(iterator);

_exit:

  YR_DEBUG_FPRINTF(2, stderr, "} = %p // %s()\n", result, __FUNCTION__);

  return result;
}