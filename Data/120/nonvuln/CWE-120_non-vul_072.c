int yr_parser_reduce_meta_declaration(
    yyscan_t yyscanner,
    int32_t type,
    const char* identifier,
    const char* string,
    int64_t integer,
    YR_ARENA_REF* meta_ref)
{
  YR_ARENA_REF ref;
  YR_COMPILER* compiler = yyget_extra(yyscanner);

  FAIL_ON_ERROR(yr_arena_allocate_struct(
      compiler->arena,
      YR_METAS_TABLE,
      sizeof(YR_META),
      meta_ref,
      offsetof(YR_META, identifier),
      offsetof(YR_META, string),
      EOL));

  YR_META* meta = (YR_META*) yr_arena_ref_to_ptr(compiler->arena, meta_ref);

  meta->type = type;
  meta->integer = integer;

  FAIL_ON_ERROR(_yr_compiler_store_string(compiler, identifier, &ref));

  meta->identifier = (const char*) yr_arena_ref_to_ptr(compiler->arena, &ref);

  if (string != NULL)
  {
    FAIL_ON_ERROR(_yr_compiler_store_string(compiler, string, &ref));

    meta->string = (const char*) yr_arena_ref_to_ptr(compiler->arena, &ref);
  }
  else
  {
    meta->string = NULL;
  }

  compiler->current_meta_idx++;

  return ERROR_SUCCESS;
}