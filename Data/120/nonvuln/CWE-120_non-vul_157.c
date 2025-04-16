YR_API int yr_scanner_define_integer_variable(
    YR_SCANNER* scanner,
    const char* identifier,
    int64_t value)
{
  YR_OBJECT* obj = (YR_OBJECT*) yr_hash_table_lookup(
      scanner->objects_table, identifier, NULL);

  if (obj == NULL)
    return ERROR_INVALID_ARGUMENT;

  if (obj->type != OBJECT_TYPE_INTEGER)
    return ERROR_INVALID_EXTERNAL_VARIABLE_TYPE;

  return yr_object_set_integer(value, obj, NULL);
}