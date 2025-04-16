static void print_rules_stats(YR_RULES* rules)
{
  YR_RULES_STATS stats;

  int t = sizeof(stats.top_ac_match_list_lengths) /
          sizeof(stats.top_ac_match_list_lengths[0]);

  int result = yr_rules_get_stats(rules, &stats);

  if (result != ERROR_SUCCESS)
  {
    print_error(result);
    return;
  }

  _tprintf(
      _T("size of AC transition table        : %d\n"), stats.ac_tables_size);

  _tprintf(
      _T("average length of AC matches lists : %f\n"),
      stats.ac_average_match_list_length);

  _tprintf(_T("number of rules                    : %d\n"), stats.num_rules);

  _tprintf(_T("number of strings                  : %d\n"), stats.num_strings);

  _tprintf(_T("number of AC matches               : %d\n"), stats.ac_matches);

  _tprintf(
      _T("number of AC matches in root node  : %d\n"),
      stats.ac_root_match_list_length);

  _tprintf(_T("number of AC matches in top %d longest lists\n"), t);

  for (int i = 0; i < t; i++)
    _tprintf(_T(" %3d: %d\n"), i + 1, stats.top_ac_match_list_lengths[i]);

  _tprintf(_T("match list length percentiles\n"));

  for (int i = 100; i >= 0; i--)
    _tprintf(_T(" %3d: %d\n"), i, stats.ac_match_list_length_pctls[i]);
}