YR_API int yr_compiler_add_string(
    YR_COMPILER* compiler,
    const char* rules_string,
    const char* namespace_)
{
  // Don't allow calls to yr_compiler_add_string() after
  // yr_compiler_get_rules() has been called.
  assert(compiler->rules == NULL);

  // Don't allow calls to yr_compiler_add_string() if a previous call to
  // yr_compiler_add_XXXX failed.
  assert(compiler->errors == 0);

  if (namespace_ != NULL)
    compiler->last_error = _yr_compiler_set_namespace(compiler, namespace_);
  else
    compiler->last_error = _yr_compiler_set_namespace(compiler, "default");

  if (compiler->last_error != ERROR_SUCCESS)
    return ++compiler->errors;

  return yr_lex_parse_rules_string(rules_string, compiler);
}