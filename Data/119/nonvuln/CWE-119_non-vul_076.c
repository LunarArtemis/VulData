void
#if defined (PREFER_STDARG)
itrace (const char *format, ...)
#else
itrace (format, va_alist)
     const char *format;
     va_dcl
#endif
{
  va_list args;

  fprintf(stderr, "TRACE: pid %ld: ", (long)getpid());

  SH_VA_START (args, format);

  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");

  va_end (args);

  fflush(stderr);
}