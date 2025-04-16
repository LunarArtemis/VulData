YR_API const uint8_t* yr_process_fetch_memory_block_data(YR_MEMORY_BLOCK* block)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) block->context;
  YR_PROC_INFO* proc_info = context->proc_info;
  vm_size_t size = block->size;

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

  if (vm_read_overwrite(
          proc_info->task,
          (vm_address_t) block->base,
          block->size,
          (vm_address_t) context->buffer,
          &size) != KERN_SUCCESS)
  {
    return NULL;
  }

  return context->buffer;
}