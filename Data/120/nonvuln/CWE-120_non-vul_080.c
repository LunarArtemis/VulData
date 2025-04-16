int yr_parser_emit_with_arg_int32(
    yyscan_t yyscanner,
    uint8_t instruction,
    int32_t argument,
    YR_ARENA_REF* instruction_ref,
    YR_ARENA_REF* argument_ref)
{
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
        &argument,
        sizeof(int32_t),
        argument_ref);

  return result;
}