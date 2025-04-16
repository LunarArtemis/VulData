int _yr_process_detach(YR_PROC_ITERATOR_CTX* context)
{
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) context->proc_info;

  CloseHandle(proc_info->hProcess);
  return ERROR_SUCCESS;
}