YR_API int yr_compiler_get_rules(YR_COMPILER* compiler, YR_RULES** rules)
{
  // Don't allow calls to yr_compiler_get_rules() if a previous call to
  // yr_compiler_add_XXXX failed.
  assert(compiler->errors == 0);

  *rules = NULL;

  if (compiler->rules == NULL)
    FAIL_ON_ERROR(_yr_compiler_compile_rules(compiler));

  *rules = compiler->rules;

  return ERROR_SUCCESS;
}