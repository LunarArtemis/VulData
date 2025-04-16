int yr_parser_emit(
    yyscan_t yyscanner,
    uint8_t instruction,
    YR_ARENA_REF* instruction_ref)
{
  return yr_arena_write_data(
      yyget_extra(yyscanner)->arena,
      YR_CODE_SECTION,
      &instruction,
      sizeof(uint8_t),
      instruction_ref);
}