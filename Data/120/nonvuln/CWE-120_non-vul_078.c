int yr_parser_lookup_string(
    yyscan_t yyscanner,
    const char* identifier,
    YR_STRING** string)
{
  YR_COMPILER* compiler = yyget_extra(yyscanner);

  YR_RULE* current_rule = _yr_compiler_get_rule_by_idx(
      compiler, compiler->current_rule_idx);

  yr_rule_strings_foreach(current_rule, *string)
  {
    // If some string $a gets fragmented into multiple chained
    // strings, all those fragments have the same $a identifier
    // but we are interested in the heading fragment, which is
    // that with chained_to == NULL

    if ((*string)->chained_to == NULL &&
        strcmp((*string)->identifier, identifier) == 0)
    {
      return ERROR_SUCCESS;
    }
  }

  yr_compiler_set_error_extra_info(compiler, identifier)

      * string = NULL;

  return ERROR_UNDEFINED_STRING;
}