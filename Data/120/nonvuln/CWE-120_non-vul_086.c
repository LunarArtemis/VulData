static int _yr_compiler_define_variable(
    YR_COMPILER* compiler,
    YR_EXTERNAL_VARIABLE* external)
{
  YR_EXTERNAL_VARIABLE* ext;
  YR_OBJECT* object;

  if (external->identifier == NULL)
    return ERROR_INVALID_ARGUMENT;

  object = (YR_OBJECT*) yr_hash_table_lookup(
      compiler->objects_table, external->identifier, NULL);

  if (object != NULL)
    return ERROR_DUPLICATED_EXTERNAL_VARIABLE;

  YR_ARENA_REF ext_ref;
  YR_ARENA_REF ref;

  FAIL_ON_ERROR(yr_arena_allocate_struct(
      compiler->arena,
      YR_EXTERNAL_VARIABLES_TABLE,
      sizeof(YR_EXTERNAL_VARIABLE),
      &ext_ref,
      offsetof(YR_EXTERNAL_VARIABLE, identifier),
      EOL));

  ext = (YR_EXTERNAL_VARIABLE*) yr_arena_ref_to_ptr(compiler->arena, &ext_ref);

  FAIL_ON_ERROR(
      _yr_compiler_store_string(compiler, external->identifier, &ref));

  ext->identifier = (const char*) yr_arena_ref_to_ptr(compiler->arena, &ref);

  ext->type = external->type;
  ext->value = external->value;

  if (external->type == EXTERNAL_VARIABLE_TYPE_STRING)
  {
    if (external->value.s == NULL)
      return ERROR_INVALID_ARGUMENT;

    FAIL_ON_ERROR(_yr_compiler_store_string(compiler, external->value.s, &ref));

    FAIL_ON_ERROR(yr_arena_make_ptr_relocatable(
        compiler->arena,
        YR_EXTERNAL_VARIABLES_TABLE,
        ext_ref.offset + offsetof(YR_EXTERNAL_VARIABLE, value.s),
        EOL));

    ext->value.s = (char*) yr_arena_ref_to_ptr(compiler->arena, &ref);
  }

  FAIL_ON_ERROR(yr_object_from_external_variable(external, &object));

  FAIL_ON_ERROR_WITH_CLEANUP(
      yr_hash_table_add(
          compiler->objects_table, external->identifier, NULL, (void*) object),
      yr_object_destroy(object));

  return ERROR_SUCCESS;
}