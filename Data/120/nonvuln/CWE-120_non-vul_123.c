static bool is_directory(const char_t* path)
{
  DWORD attributes = GetFileAttributes(path);

  if (attributes != INVALID_FILE_ATTRIBUTES &&
      attributes & FILE_ATTRIBUTE_DIRECTORY)
    return true;
  else
    return false;
}