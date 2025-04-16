int _yr_process_attach(int pid, YR_PROC_ITERATOR_CTX* context)
{
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) yr_malloc(sizeof(YR_PROC_INFO));

  if (proc_info == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  kern_return_t kr = task_for_pid(mach_task_self(), pid, &proc_info->task);

  if (kr != KERN_SUCCESS)
  {
    yr_free(proc_info);
    return ERROR_COULD_NOT_ATTACH_TO_PROCESS;
  }

  context->proc_info = proc_info;

  return ERROR_SUCCESS;
}