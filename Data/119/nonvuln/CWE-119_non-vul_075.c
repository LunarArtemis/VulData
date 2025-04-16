void
#if defined (PREFER_STDARG)
trace (const char *format, ...)
#else
trace (format, va_alist)
     const char *format;
     va_dcl
#endif
{
  va_list args;
  static FILE *tracefp = (FILE *)NULL;

  if (tracefp == NULL)
    tracefp = fopen("/tmp/bash-trace.log", "a+");

  if (tracefp == NULL)
    tracefp = stderr;
  else
    fcntl (fileno (tracefp), F_SETFD, 1);     /* close-on-exec */

  fprintf(tracefp, "TRACE: pid %ld: ", (long)getpid());

  SH_VA_START (args, format);

  vfprintf (tracefp, format, args);
  fprintf (tracefp, "\n");

  va_end (args);

  fflush(tracefp);
}