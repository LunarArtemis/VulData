YR_RULE* _yr_compiler_get_rule_by_idx(YR_COMPILER* compiler, uint32_t rule_idx)
{
  return (YR_RULE*) yr_arena_get_ptr(
      compiler->arena, YR_RULES_TABLE, rule_idx * sizeof(YR_RULE));
}