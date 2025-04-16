static int _yr_parser_valid_module_name(SIZED_STRING* module_name)
{
  if (module_name->length == 0)
    return false;

  if (strlen(module_name->c_string) != module_name->length)
    return false;

  return true;
}