static int scan_file(YR_SCANNER* scanner, const char_t* filename)
{
  YR_FILE_DESCRIPTOR fd = CreateFile(
      filename,
      GENERIC_READ,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_SEQUENTIAL_SCAN,
      NULL);

  if (fd == INVALID_HANDLE_VALUE)
    return ERROR_COULD_NOT_OPEN_FILE;

  int result = yr_scanner_scan_fd(scanner, fd);

  CloseHandle(fd);

  return result;
}