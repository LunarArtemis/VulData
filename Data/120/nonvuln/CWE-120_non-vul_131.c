YR_API const uint8_t* yr_process_fetch_memory_block_data(YR_MEMORY_BLOCK* block)
{
  SIZE_T read;

  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) block->context;
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  if (context->buffer_size < block->size)
  {
    if (context->buffer != NULL)
      yr_free((void*) context->buffer);

    context->buffer = (const uint8_t*) yr_malloc(block->size);

    if (context->buffer != NULL)
    {
      context->buffer_size = block->size;
    }
    else
    {
      context->buffer_size = 0;
      return NULL;
    }
  }

  if (ReadProcessMemory(
          proc_info->hProcess,
          (LPCVOID) block->base,
          (LPVOID) context->buffer,
          (SIZE_T) block->size,
          &read) == FALSE)
  {
    return NULL;
  }

  return context->buffer;
}