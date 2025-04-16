char *
get_name_for_error ()
{
  char *name;
#if defined (ARRAY_VARS)
  SHELL_VAR *bash_source_v;
  ARRAY *bash_source_a;
#endif

  name = (char *)NULL;
  if (interactive_shell == 0)
    {
#if defined (ARRAY_VARS)
      bash_source_v = find_variable ("BASH_SOURCE");
      if (bash_source_v && array_p (bash_source_v) &&
	  (bash_source_a = array_cell (bash_source_v)))
	name = array_reference (bash_source_a, 0);
      if (name == 0 || *name == '\0')	/* XXX - was just name == 0 */
#endif
	name = dollar_vars[0];
    }
  if (name == 0 && shell_name && *shell_name)
    name = base_pathname (shell_name);
  if (name == 0)
#if defined (PROGRAM)
    name = PROGRAM;
#else
    name = "bash";
#endif

  return (name);
}