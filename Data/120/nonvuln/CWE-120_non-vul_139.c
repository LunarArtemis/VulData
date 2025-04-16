YR_API const uint8_t* yr_process_fetch_memory_block_data(YR_MEMORY_BLOCK* block)
{
  YR_PROC_ITERATOR_CTX* context = (YR_PROC_ITERATOR_CTX*) block->context;
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  struct ptrace_io_desc io_desc;

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

  io_desc.piod_op = PIOD_READ_D;
  io_desc.piod_offs = (void*) block->base;
  io_desc.piod_addr = (void*) context->buffer;
  io_desc.piod_len = block->size;

  if (ptrace(PT_IO, proc_info->pid, (char*) &io_desc, 0) == -1)
    return NULL;

  return context->buffer;
}