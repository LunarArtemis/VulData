int yr_parser_check_types(
    YR_COMPILER* compiler,
    YR_OBJECT_FUNCTION* function,
    const char* actual_args_fmt)
{
  int i;

  for (i = 0; i < YR_MAX_OVERLOADED_FUNCTIONS; i++)
  {
    if (function->prototypes[i].arguments_fmt == NULL)
      break;

    if (strcmp(function->prototypes[i].arguments_fmt, actual_args_fmt) == 0)
      return ERROR_SUCCESS;
  }

  yr_compiler_set_error_extra_info(compiler, function->identifier)

      return ERROR_WRONG_ARGUMENTS;
}