YR_API YR_RULE* yr_scanner_last_error_rule(YR_SCANNER* scanner)
{
  if (scanner->last_error_string == NULL)
    return NULL;

  return &scanner->rules->rules_table[scanner->last_error_string->rule_idx];
}