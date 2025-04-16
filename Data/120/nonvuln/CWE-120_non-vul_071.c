int yr_parser_emit_with_arg_reloc(
    yyscan_t yyscanner,
    uint8_t instruction,
    void* argument,
    YR_ARENA_REF* instruction_ref,
    YR_ARENA_REF* argument_ref)
{
  YR_ARENA_REF ref = YR_ARENA_NULL_REF;

  DECLARE_REFERENCE(void*, ptr) arg;

  memset(&arg, 0, sizeof(arg));
  arg.ptr = argument;

  int result = yr_arena_write_data(
      yyget_extra(yyscanner)->arena,
      YR_CODE_SECTION,
      &instruction,
      sizeof(uint8_t),
      instruction_ref);

  if (result == ERROR_SUCCESS)
    result = yr_arena_write_data(
        yyget_extra(yyscanner)->arena,
        YR_CODE_SECTION,
        &arg,
        sizeof(arg),
        &ref);

  if (result == ERROR_SUCCESS)
    result = yr_arena_make_ptr_relocatable(
        yyget_extra(yyscanner)->arena, YR_CODE_SECTION, ref.offset, EOL);

  if (argument_ref != NULL)
    *argument_ref = ref;

  return result;
}