int yr_parser_reduce_import(yyscan_t yyscanner, SIZED_STRING* module_name)
{
  int result;

  YR_ARENA_REF ref;
  YR_COMPILER* compiler = yyget_extra(yyscanner);
  YR_OBJECT* module_structure;

  if (!_yr_parser_valid_module_name(module_name))
  {
    yr_compiler_set_error_extra_info(compiler, module_name->c_string);

    return ERROR_INVALID_MODULE_NAME;
  }

  YR_NAMESPACE* ns = (YR_NAMESPACE*) yr_arena_get_ptr(
      compiler->arena,
      YR_NAMESPACES_TABLE,
      compiler->current_namespace_idx * sizeof(struct YR_NAMESPACE));

  module_structure = (YR_OBJECT*) yr_hash_table_lookup(
      compiler->objects_table, module_name->c_string, ns->name);

  // if module already imported, do nothing

  if (module_structure != NULL)
    return ERROR_SUCCESS;

  FAIL_ON_ERROR(yr_object_create(
      OBJECT_TYPE_STRUCTURE, module_name->c_string, NULL, &module_structure));

  FAIL_ON_ERROR(yr_hash_table_add(
      compiler->objects_table,
      module_name->c_string,
      ns->name,
      module_structure));

  result = yr_modules_do_declarations(module_name->c_string, module_structure);

  if (result == ERROR_UNKNOWN_MODULE)
    yr_compiler_set_error_extra_info(compiler, module_name->c_string);

  if (result != ERROR_SUCCESS)
    return result;

  FAIL_ON_ERROR(
      _yr_compiler_store_string(compiler, module_name->c_string, &ref));

  FAIL_ON_ERROR(yr_parser_emit_with_arg_reloc(
      yyscanner,
      OP_IMPORT,
      yr_arena_ref_to_ptr(compiler->arena, &ref),
      NULL,
      NULL));

  return ERROR_SUCCESS;
}