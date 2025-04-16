const char* _yr_compiler_default_include_callback(
    const char* include_name,
    const char* calling_rule_filename,
    const char* calling_rule_namespace,
    void* user_data)
{
#ifndef _MSC_VER
  struct stat stbuf;
#endif

  char* file_buffer;

#ifdef _MSC_VER
  long file_size;
#else
  off_t file_size;
#endif

  int fd = -1;

#if defined(_MSC_VER)
  _sopen_s(&fd, include_name, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD);
#elif defined(_WIN32) || defined(__CYGWIN__)
  fd = open(include_name, O_RDONLY | O_BINARY);
#else
  fd = open(include_name, O_RDONLY);
#endif

  if (fd == -1)
    return NULL;

#ifdef _MSC_VER
  file_size = _filelength(fd);
  if (file_size == -1)
  {
    _close(fd);
    return NULL;
  }
#else
  if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
  {
    close(fd);
    return NULL;
  }
  file_size = stbuf.st_size;
#endif

  file_buffer = (char*) yr_malloc((size_t) file_size + 1);

  if (file_buffer == NULL)
  {
#ifdef _MSC_VER
    _close(fd);
#else
    close(fd);
#endif

    return NULL;
  }

  if (file_size != read(fd, file_buffer, (size_t) file_size))
  {
    yr_free(file_buffer);

#ifdef _MSC_VER
    _close(fd);
#else
    close(fd);
#endif

    return NULL;
  }
  else
  {
    file_buffer[file_size] = '\0';
  }

#ifdef _MSC_VER
  _close(fd);
#else
  close(fd);
#endif

  return file_buffer;
}