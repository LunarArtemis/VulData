void
#if defined (PREFER_STDARG)
parser_error (int lineno, const char *format, ...)
#else
parser_error (lineno, format, va_alist)
     int lineno;
     const char *format;
     va_dcl
#endif
{
  va_list args;
  char *ename, *iname;

  ename = get_name_for_error ();
  iname = yy_input_name ();

  if (interactive)
    fprintf (stderr, "%s: ", ename);
  else if (interactive_shell)
    fprintf (stderr, "%s: %s:%s%d: ", ename, iname, gnu_error_format ? "" : _(" line "), lineno);
  else if (STREQ (ename, iname))
    fprintf (stderr, "%s:%s%d: ", ename, gnu_error_format ? "" : _(" line "), lineno);
  else
    fprintf (stderr, "%s: %s:%s%d: ", ename, iname, gnu_error_format ? "" : _(" line "), lineno);

  SH_VA_START (args, format);

  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");

  va_end (args);

  if (exit_immediately_on_error)
    exit_shell (last_command_exit_value = 2);
}