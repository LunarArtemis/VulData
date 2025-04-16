int _yr_process_detach(YR_PROC_ITERATOR_CTX* context)
{
  YR_PROC_INFO* proc_info = context->proc_info;

  if (proc_info->task != MACH_PORT_NULL)
    mach_port_deallocate(mach_task_self(), proc_info->task);

  return ERROR_SUCCESS;
}