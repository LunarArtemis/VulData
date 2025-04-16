int yr_parser_emit_pushes_for_strings(
    yyscan_t yyscanner,
    const char* identifier)
{
  YR_COMPILER* compiler = yyget_extra(yyscanner);

  YR_RULE* current_rule = _yr_compiler_get_rule_by_idx(
      compiler, compiler->current_rule_idx);

  YR_STRING* string;

  const char* string_identifier;
  const char* target_identifier;

  int matching = 0;

  yr_rule_strings_foreach(current_rule, string)
  {
    // Don't generate pushes for strings chained to another one, we are
    // only interested in non-chained strings or the head of the chain.

    if (string->chained_to == NULL)
    {
      string_identifier = string->identifier;
      target_identifier = identifier;

      while (*target_identifier != '\0' && *string_identifier != '\0' &&
             *target_identifier == *string_identifier)
      {
        target_identifier++;
        string_identifier++;
      }

      if ((*target_identifier == '\0' && *string_identifier == '\0') ||
          *target_identifier == '*')
      {
        yr_parser_emit_with_arg_reloc(yyscanner, OP_PUSH, string, NULL, NULL);

        string->flags |= STRING_FLAGS_REFERENCED;
        string->flags &= ~STRING_FLAGS_FIXED_OFFSET;
        matching++;
      }
    }
  }

  if (matching == 0)
  {
    yr_compiler_set_error_extra_info(
        compiler, identifier) return ERROR_UNDEFINED_STRING;
  }

  return ERROR_SUCCESS;
}