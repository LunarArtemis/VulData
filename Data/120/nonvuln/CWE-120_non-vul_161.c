YR_API int yr_scanner_scan_fd(YR_SCANNER* scanner, YR_FILE_DESCRIPTOR fd)
{
  YR_MAPPED_FILE mfile;

  int result = yr_filemap_map_fd(fd, 0, 0, &mfile);

  if (result == ERROR_SUCCESS)
  {
    result = yr_scanner_scan_mem(scanner, mfile.data, mfile.size);
    yr_filemap_unmap_fd(&mfile);
  }

  return result;
}