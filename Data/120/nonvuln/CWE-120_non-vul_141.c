int _yr_process_detach(YR_PROC_ITERATOR_CTX* context)
{
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  ptrace(PT_DETACH, proc_info->pid, NULL, 0);

  return ERROR_SUCCESS;
}