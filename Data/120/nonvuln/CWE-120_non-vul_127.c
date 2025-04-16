static int scan_dir(const char_t* dir, SCAN_OPTIONS* scan_opts)
{
  int result = ERROR_SUCCESS;
  char_t path[MAX_PATH];

  _sntprintf(path, MAX_PATH, _T("%s\\*"), dir);

  WIN32_FIND_DATA FindFileData;
  HANDLE hFind = FindFirstFile(path, &FindFileData);

  if (hFind != INVALID_HANDLE_VALUE)
  {
    do
    {
      _sntprintf(path, MAX_PATH, _T("%s\\%s"), dir, FindFileData.cFileName);

      if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        result = file_queue_put(path, scan_opts->deadline);
      }
      else if (
          scan_opts->recursive_search &&
          _tcscmp(FindFileData.cFileName, _T(".")) != 0 &&
          _tcscmp(FindFileData.cFileName, _T("..")) != 0)
      {
        result = scan_dir(path, scan_opts);
      }

    } while (result != ERROR_SCAN_TIMEOUT &&
             FindNextFile(hFind, &FindFileData));

    FindClose(hFind);
  }

  return result;
}