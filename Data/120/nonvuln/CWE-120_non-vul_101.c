static int _yr_compiler_compile_rules(YR_COMPILER* compiler)
{
  YR_RULE null_rule;
  YR_EXTERNAL_VARIABLE null_external;

  uint8_t halt = OP_HALT;

  // Write halt instruction at the end of code.
  FAIL_ON_ERROR(yr_arena_write_data(
      compiler->arena, YR_CODE_SECTION, &halt, sizeof(uint8_t), NULL));

  // Write a null rule indicating the end.
  memset(&null_rule, 0xFA, sizeof(YR_RULE));
  null_rule.flags = RULE_FLAGS_NULL;

  FAIL_ON_ERROR(yr_arena_write_data(
      compiler->arena, YR_RULES_TABLE, &null_rule, sizeof(YR_RULE), NULL));

  // Write a null external indicating the end.
  memset(&null_external, 0xFA, sizeof(YR_EXTERNAL_VARIABLE));
  null_external.type = EXTERNAL_VARIABLE_TYPE_NULL;

  FAIL_ON_ERROR(yr_arena_write_data(
      compiler->arena,
      YR_EXTERNAL_VARIABLES_TABLE,
      &null_external,
      sizeof(YR_EXTERNAL_VARIABLE),
      NULL));

  // Write Aho-Corasick automaton to arena.
  FAIL_ON_ERROR(yr_ac_compile(compiler->automaton, compiler->arena));

  YR_ARENA_REF ref;

  FAIL_ON_ERROR(yr_arena_allocate_struct(
      compiler->arena, YR_SUMMARY_SECTION, sizeof(YR_SUMMARY), &ref, EOL));

  YR_SUMMARY* summary = (YR_SUMMARY*) yr_arena_ref_to_ptr(
      compiler->arena, &ref);

  summary->num_namespaces = compiler->num_namespaces;
  summary->num_rules = compiler->next_rule_idx;
  summary->num_strings = compiler->current_string_idx;

  return yr_rules_from_arena(compiler->arena, &compiler->rules);
}