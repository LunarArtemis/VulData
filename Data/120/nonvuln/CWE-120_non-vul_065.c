int yr_parser_reduce_string_identifier(
    yyscan_t yyscanner,
    const char* identifier,
    uint8_t instruction,
    uint64_t at_offset)
{
  YR_STRING* string;
  YR_COMPILER* compiler = yyget_extra(yyscanner);

  if (strcmp(identifier, "$") == 0)  // is an anonymous string ?
  {
    if (compiler->loop_for_of_var_index >= 0)  // inside a loop ?
    {
      yr_parser_emit_with_arg(
          yyscanner, OP_PUSH_M, compiler->loop_for_of_var_index, NULL, NULL);

      yr_parser_emit(yyscanner, instruction, NULL);

      YR_RULE* current_rule = _yr_compiler_get_rule_by_idx(
          compiler, compiler->current_rule_idx);

      yr_rule_strings_foreach(current_rule, string)
      {
        if (instruction != OP_FOUND)
          string->flags &= ~STRING_FLAGS_SINGLE_MATCH;

        if (instruction == OP_FOUND_AT)
        {
          // Avoid overwriting any previous fixed offset
          if (string->fixed_offset == YR_UNDEFINED)
            string->fixed_offset = at_offset;

          // If a previous fixed offset was different, disable
          // the STRING_GFLAGS_FIXED_OFFSET flag because we only
          // have room to store a single fixed offset value
          if (string->fixed_offset != at_offset)
            string->flags &= ~STRING_FLAGS_FIXED_OFFSET;
        }
        else
        {
          string->flags &= ~STRING_FLAGS_FIXED_OFFSET;
        }
      }
    }
    else
    {
      // Anonymous strings not allowed outside of a loop
      return ERROR_MISPLACED_ANONYMOUS_STRING;
    }
  }
  else
  {
    FAIL_ON_ERROR(yr_parser_lookup_string(yyscanner, identifier, &string));

    FAIL_ON_ERROR(
        yr_parser_emit_with_arg_reloc(yyscanner, OP_PUSH, string, NULL, NULL));

    if (instruction != OP_FOUND)
      string->flags &= ~STRING_FLAGS_SINGLE_MATCH;

    if (instruction == OP_FOUND_AT)
    {
      // Avoid overwriting any previous fixed offset

      if (string->fixed_offset == YR_UNDEFINED)
        string->fixed_offset = at_offset;

      // If a previous fixed offset was different, disable
      // the STRING_GFLAGS_FIXED_OFFSET flag because we only
      // have room to store a single fixed offset value

      if (string->fixed_offset == YR_UNDEFINED ||
          string->fixed_offset != at_offset)
      {
        string->flags &= ~STRING_FLAGS_FIXED_OFFSET;
      }
    }
    else
    {
      string->flags &= ~STRING_FLAGS_FIXED_OFFSET;
    }

    FAIL_ON_ERROR(yr_parser_emit(yyscanner, instruction, NULL));

    string->flags |= STRING_FLAGS_REFERENCED;
  }

  return ERROR_SUCCESS;
}