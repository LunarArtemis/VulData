int _yr_process_detach(YR_PROC_ITERATOR_CTX* context)
{
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;
  if (proc_info)
  {
    fclose(proc_info->maps);
    close(proc_info->mem_fd);
    close(proc_info->pagemap_fd);
  }

  if (context->buffer != NULL)
  {
    munmap((void*) context->buffer, context->buffer_size);
    context->buffer = NULL;
    context->buffer_size = 0;
  }

  return ERROR_SUCCESS;
}