void
#if defined (PREFER_STDARG)
programming_error (const char *format, ...)
#else
programming_error (format, va_alist)
     const char *format;
     va_dcl
#endif
{
  va_list args;
  char *h;

#if defined (JOB_CONTROL)
  give_terminal_to (shell_pgrp, 0);
#endif /* JOB_CONTROL */

  SH_VA_START (args, format);

  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");
  va_end (args);

#if defined (HISTORY)
  if (remember_on_history)
    {
      h = last_history_line ();
      fprintf (stderr, _("last command: %s\n"), h ? h : "(null)");
    }
#endif

#if 0
  fprintf (stderr, "Report this to %s\n", the_current_maintainer);
#endif

  fprintf (stderr, _("Aborting..."));
  fflush (stderr);

  abort ();
}