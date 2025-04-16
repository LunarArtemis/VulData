YR_API int yr_compiler_define_float_variable(
    YR_COMPILER* compiler,
    const char* identifier,
    double value)
{
  YR_EXTERNAL_VARIABLE external;

  external.type = EXTERNAL_VARIABLE_TYPE_FLOAT;
  external.identifier = identifier;
  external.value.f = value;

  FAIL_ON_ERROR(_yr_compiler_define_variable(compiler, &external));

  return ERROR_SUCCESS;
}