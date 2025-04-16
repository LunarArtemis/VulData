static int sort_by_cost_desc(
    const struct YR_RULE_PROFILING_INFO* r1,
    const struct YR_RULE_PROFILING_INFO* r2)
{
  if (r1->cost < r2->cost)
    return 1;

  if (r1->cost > r2->cost)
    return -1;

  return 0;
}