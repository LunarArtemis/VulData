static void _yr_scanner_clean_matches(YR_SCANNER* scanner)
{
  YR_DEBUG_FPRINTF(2, stderr, "- %s() {} \n", __FUNCTION__);

  memset(
      scanner->rule_matches_flags,
      0,
      sizeof(YR_BITMASK) * YR_BITMASK_SIZE(scanner->rules->num_rules));

  memset(
      scanner->ns_unsatisfied_flags,
      0,
      sizeof(YR_BITMASK) * YR_BITMASK_SIZE(scanner->rules->num_namespaces));

  memset(
      scanner->strings_temp_disabled,
      0,
      sizeof(YR_BITMASK) * YR_BITMASK_SIZE(scanner->rules->num_strings));

  memset(scanner->matches, 0, sizeof(YR_MATCHES) * scanner->rules->num_strings);

  memset(
      scanner->unconfirmed_matches,
      0,
      sizeof(YR_MATCHES) * scanner->rules->num_strings);
}