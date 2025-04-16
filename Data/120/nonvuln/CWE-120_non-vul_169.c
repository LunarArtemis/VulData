YR_API int yr_scanner_scan_proc(YR_SCANNER* scanner, int pid)
{
  YR_MEMORY_BLOCK_ITERATOR iterator;

  int result = yr_process_open_iterator(pid, &iterator);

  if (result == ERROR_SUCCESS)
  {
    int prev_flags = scanner->flags;
    scanner->flags |= SCAN_FLAGS_PROCESS_MEMORY;
    result = yr_scanner_scan_mem_blocks(scanner, &iterator);
    scanner->flags = prev_flags;
    yr_process_close_iterator(&iterator);
  }

  return result;
}