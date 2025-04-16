int yr_parser_reduce_rule_declaration_phase_1(
    yyscan_t yyscanner,
    int32_t flags,
    const char* identifier,
    YR_ARENA_REF* rule_ref)
{
  YR_FIXUP* fixup;
  YR_COMPILER* compiler = yyget_extra(yyscanner);

  YR_NAMESPACE* ns = (YR_NAMESPACE*) yr_arena_get_ptr(
      compiler->arena,
      YR_NAMESPACES_TABLE,
      compiler->current_namespace_idx * sizeof(struct YR_NAMESPACE));

  if (yr_hash_table_lookup_uint32(
          compiler->rules_table, identifier, ns->name) != UINT32_MAX ||
      yr_hash_table_lookup(compiler->objects_table, identifier, NULL) != NULL)
  {
    // A rule or variable with the same identifier already exists, return the
    // appropriate error.

    yr_compiler_set_error_extra_info(
        compiler, identifier) return ERROR_DUPLICATED_IDENTIFIER;
  }

  FAIL_ON_ERROR(yr_arena_allocate_struct(
      compiler->arena,
      YR_RULES_TABLE,
      sizeof(YR_RULE),
      rule_ref,
      offsetof(YR_RULE, identifier),
      offsetof(YR_RULE, tags),
      offsetof(YR_RULE, strings),
      offsetof(YR_RULE, metas),
      offsetof(YR_RULE, ns),
      EOL));

  YR_RULE* rule = (YR_RULE*) yr_arena_ref_to_ptr(compiler->arena, rule_ref);

  YR_ARENA_REF ref;

  FAIL_ON_ERROR(_yr_compiler_store_string(compiler, identifier, &ref));

  rule->identifier = (const char*) yr_arena_ref_to_ptr(compiler->arena, &ref);
  rule->flags = flags;
  rule->ns = ns;
  rule->num_atoms = 0;

  YR_ARENA_REF jmp_offset_ref;

  // We are starting to parse a new rule, set current_rule_idx accordingly.
  compiler->current_rule_idx = compiler->next_rule_idx;
  compiler->next_rule_idx++;

  // The OP_INIT_RULE instruction behaves like a jump. When the rule is disabled
  // it skips over the rule's code and go straight to the next rule's code. The
  // jmp_offset_ref variable points to the jump's offset. The offset is set to 0
  // as we don't know the jump target yet. When we finish generating the rule's
  // code in yr_parser_reduce_rule_declaration_phase_2 the jump offset is set to
  // its final value.

  FAIL_ON_ERROR(yr_parser_emit_with_arg_int32(
      yyscanner, OP_INIT_RULE, 0, NULL, &jmp_offset_ref));

  FAIL_ON_ERROR(yr_arena_write_data(
      compiler->arena,
      YR_CODE_SECTION,
      &compiler->current_rule_idx,
      sizeof(compiler->current_rule_idx),
      NULL));

  // Create a fixup entry for the jump and push it in the stack
  fixup = (YR_FIXUP*) yr_malloc(sizeof(YR_FIXUP));

  if (fixup == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  fixup->ref = jmp_offset_ref;
  fixup->next = compiler->fixup_stack_head;
  compiler->fixup_stack_head = fixup;

  // Clean strings_table as we are starting to parse a new rule.
  yr_hash_table_clean(compiler->strings_table, NULL);

  FAIL_ON_ERROR(yr_hash_table_add_uint32(
      compiler->rules_table, identifier, ns->name, compiler->current_rule_idx));

  return ERROR_SUCCESS;
}