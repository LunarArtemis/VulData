int _yr_process_attach(int pid, YR_PROC_ITERATOR_CTX* context)
{
  int status;
  size_t len = sizeof(struct kinfo_vmentry);

  int mib[] = {CTL_KERN, KERN_PROC_VMMAP, pid};
  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) yr_malloc(sizeof(YR_PROC_INFO));

  if (proc_info == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  memset(proc_info, 0, sizeof(YR_PROC_INFO));

  proc_info->pid = pid;
  if (ptrace(PT_ATTACH, pid, NULL, 0) == -1)
  {
    yr_free(proc_info);

    return ERROR_COULD_NOT_ATTACH_TO_PROCESS;
  }

  status = 0;
  if (waitpid(pid, &status, 0) == -1)
  {
    ptrace(PT_DETACH, proc_info->pid, NULL, 0);
    yr_free(proc_info);

    return ERROR_COULD_NOT_ATTACH_TO_PROCESS;
  }

  if (sysctl(mib, 3, &proc_info->vm_entry, &len, NULL, 0) < 0)
  {
    ptrace(PT_DETACH, proc_info->pid, NULL, 0);
    yr_free(proc_info);

    return ERROR_COULD_NOT_ATTACH_TO_PROCESS;
  }

  context->proc_info = proc_info;

  return ERROR_SUCCESS;
}