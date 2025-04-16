YR_API YR_RULE_PROFILING_INFO* yr_scanner_get_profiling_info(
    YR_SCANNER* scanner)
{
  YR_RULE_PROFILING_INFO* profiling_info = yr_malloc(
      (scanner->rules->num_rules + 1) * sizeof(YR_RULE_PROFILING_INFO));

  if (profiling_info == NULL)
    return NULL;

  for (uint32_t i = 0; i < scanner->rules->num_rules; i++)
  {
    profiling_info[i].rule = &scanner->rules->rules_table[i];
#ifdef YR_PROFILING_ENABLED
    profiling_info[i].cost = scanner->profiling_info[i].exec_time +
                             (scanner->profiling_info[i].atom_matches *
                              scanner->profiling_info[i].match_time) /
                                 YR_MATCH_VERIFICATION_PROFILING_RATE;
#else
    memset(&profiling_info[i], 0, sizeof(YR_RULE_PROFILING_INFO));
#endif
  }

  qsort(
      profiling_info,
      scanner->rules->num_rules,
      sizeof(YR_RULE_PROFILING_INFO),
      (int (*)(const void*, const void*)) sort_by_cost_desc);

  profiling_info[scanner->rules->num_rules].rule = NULL;
  profiling_info[scanner->rules->num_rules].cost = 0;

  return profiling_info;
}