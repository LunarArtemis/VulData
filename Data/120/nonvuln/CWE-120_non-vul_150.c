YR_API int yr_scanner_define_string_variable(
    YR_SCANNER* scanner,
    const char* identifier,
    const char* value)
{
  YR_OBJECT* obj = (YR_OBJECT*) yr_hash_table_lookup(
      scanner->objects_table, identifier, NULL);

  if (obj == NULL)
    return ERROR_INVALID_ARGUMENT;

  if (obj->type != OBJECT_TYPE_STRING)
    return ERROR_INVALID_EXTERNAL_VARIABLE_TYPE;

  return yr_object_set_string(value, strlen(value), obj, NULL);
}