YR_API int yr_compiler_define_string_variable(
    YR_COMPILER* compiler,
    const char* identifier,
    const char* value)
{
  YR_EXTERNAL_VARIABLE external;

  external.type = EXTERNAL_VARIABLE_TYPE_STRING;
  external.identifier = identifier;
  external.value.s = (char*) value;

  FAIL_ON_ERROR(_yr_compiler_define_variable(compiler, &external));

  return ERROR_SUCCESS;
}