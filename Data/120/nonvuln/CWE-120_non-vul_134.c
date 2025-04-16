int _yr_process_attach(int pid, YR_PROC_ITERATOR_CTX* context)
{
  TOKEN_PRIVILEGES tokenPriv;
  LUID luidDebug;
  HANDLE hToken = NULL;

  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) yr_malloc(sizeof(YR_PROC_INFO));

  if (proc_info == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) &&
      LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidDebug))
  {
    tokenPriv.PrivilegeCount = 1;
    tokenPriv.Privileges[0].Luid = luidDebug;
    tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(
        hToken, FALSE, &tokenPriv, sizeof(tokenPriv), NULL, NULL);
  }

  if (hToken != NULL)
    CloseHandle(hToken);

  proc_info->hProcess = OpenProcess(
      PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);

  if (proc_info->hProcess == NULL)
  {
    yr_free(proc_info);
    return ERROR_COULD_NOT_ATTACH_TO_PROCESS;
  }

  GetSystemInfo(&proc_info->si);

  context->proc_info = proc_info;

  return ERROR_SUCCESS;
}