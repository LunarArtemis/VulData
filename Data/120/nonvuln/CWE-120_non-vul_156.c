YR_API int yr_scanner_define_boolean_variable(
    YR_SCANNER* scanner,
    const char* identifier,
    int value)
{
  return yr_scanner_define_integer_variable(scanner, identifier, value);
}