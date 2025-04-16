static int populate_scan_list(const char_t* filename, SCAN_OPTIONS* scan_opts)
{
  char_t* context;
  DWORD nread;
  int result = ERROR_SUCCESS;

  HANDLE hFile = CreateFile(
      filename,
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL);

  if (hFile == INVALID_HANDLE_VALUE)
  {
    _ftprintf(stderr, _T("error: could not open file \"%s\".\n"), filename);
    return ERROR_COULD_NOT_OPEN_FILE;
  }

  DWORD fileSize = GetFileSize(hFile, NULL);

  if (fileSize == INVALID_FILE_SIZE)
  {
    _ftprintf(
        stderr,
        _T("error: could not determine size of file \"%s\".\n"),
        filename);
    CloseHandle(hFile);
    return ERROR_COULD_NOT_READ_FILE;
  }

  // INVALID_FILE_SIZE is 0xFFFFFFFF, so (+1) will not overflow
  char_t* buf = (char_t*) VirtualAlloc(
      NULL, fileSize + 1, MEM_COMMIT, PAGE_READWRITE);

  if (buf == NULL)
  {
    _ftprintf(
        stderr,
        _T("error: could not allocate memory for file \"%s\".\n"),
        filename);
    CloseHandle(hFile);
    return ERROR_INSUFFICIENT_MEMORY;
  }

  DWORD total = 0;

  while (total < fileSize)
  {
    if (!ReadFile(hFile, buf + total, fileSize - total, &nread, NULL))
    {
      _ftprintf(stderr, _T("error: could not read file \"%s\".\n"), filename);
      CloseHandle(hFile);
      return ERROR_COULD_NOT_READ_FILE;
    }
    total += nread;
  }

  char_t* path = _tcstok_s(buf, _T("\n"), &context);

  while (result != ERROR_SCAN_TIMEOUT && path != NULL)
  {
    // Remove trailing carriage return, if present.
    if (*path != '\0')
    {
      char_t* final = path + _tcslen(path) - 1;

      if (*final == '\r')
        *final = '\0';
    }

    if (is_directory(path))
      result = scan_dir(path, scan_opts);
    else
      result = file_queue_put(path, scan_opts->deadline);

    path = _tcstok_s(NULL, _T("\n"), &context);
  }

  CloseHandle(hFile);

  return result;
}