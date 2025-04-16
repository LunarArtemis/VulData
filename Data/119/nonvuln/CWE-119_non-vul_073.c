void
#if defined (PREFER_STDARG)
sys_error (const char *format, ...)
#else
sys_error (format, va_alist)
     const char *format;
     va_dcl
#endif
{
  int e;
  va_list args;

  e = errno;
  error_prolog (0);

  SH_VA_START (args, format);

  vfprintf (stderr, format, args);
  fprintf (stderr, ": %s\n", strerror (e));

  va_end (args);
}