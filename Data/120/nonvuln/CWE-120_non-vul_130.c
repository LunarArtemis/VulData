int _yr_process_attach(int pid, YR_PROC_ITERATOR_CTX* context)
{
  char buffer[256];

  page_size = sysconf(_SC_PAGE_SIZE);
  if (page_size < 0)
    page_size = 4096;

  YR_PROC_INFO* proc_info = (YR_PROC_INFO*) yr_malloc(sizeof(YR_PROC_INFO));

  if (proc_info == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  proc_info->pid = pid;
  proc_info->maps = NULL;
  proc_info->mem_fd = -1;
  proc_info->pagemap_fd = -1;
  proc_info->next_block_end = 0;

  snprintf(buffer, sizeof(buffer), "/proc/%u/maps", pid);
  proc_info->maps = fopen(buffer, "r");

  if (proc_info->maps == NULL)
    goto err;

  snprintf(buffer, sizeof(buffer), "/proc/%u/mem", pid);
  proc_info->mem_fd = open(buffer, O_RDONLY);

  if (proc_info->mem_fd == -1)
    goto err;

  snprintf(buffer, sizeof(buffer), "/proc/%u/pagemap", pid);
  proc_info->pagemap_fd = open(buffer, O_RDONLY);

  if (proc_info->mem_fd == -1)
    goto err;

  context->proc_info = proc_info;

  return ERROR_SUCCESS;

err:
  if (proc_info)
  {
    if (proc_info->pagemap_fd != -1)
      close(proc_info->pagemap_fd);

    if (proc_info->mem_fd != -1)
      close(proc_info->mem_fd);

    if (proc_info->maps != NULL)
      fclose(proc_info->maps);

    yr_free(proc_info);
  }

  return ERROR_COULD_NOT_ATTACH_TO_PROCESS;
}