void
#if defined (PREFER_STDARG)
internal_error (const char *format, ...)
#else
internal_error (format, va_alist)
     const char *format;
     va_dcl
#endif
{
  va_list args;

  error_prolog (1);

  SH_VA_START (args, format);

  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");

  va_end (args);
}