YR_API int yr_scanner_scan_file(YR_SCANNER* scanner, const char* filename)
{
  YR_MAPPED_FILE mfile;

  int result = yr_filemap_map(filename, &mfile);

  if (result == ERROR_SUCCESS)
  {
    result = yr_scanner_scan_mem(scanner, mfile.data, mfile.size);
    yr_filemap_unmap(&mfile);
  }

  return result;
}