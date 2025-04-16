int yr_parser_reduce_rule_declaration_phase_2(
    yyscan_t yyscanner,
    YR_ARENA_REF* rule_ref)
{
  uint32_t max_strings_per_rule;
  uint32_t strings_in_rule = 0;

  YR_FIXUP* fixup;
  YR_STRING* string;
  YR_COMPILER* compiler = yyget_extra(yyscanner);

  yr_get_configuration(
      YR_CONFIG_MAX_STRINGS_PER_RULE, (void*) &max_strings_per_rule);

  YR_RULE* rule = (YR_RULE*) yr_arena_ref_to_ptr(compiler->arena, rule_ref);

  // Show warning if the rule is generating too many atoms. The warning is
  // shown if the number of atoms is greater than 20 times the maximum number
  // of strings allowed for a rule, as 20 is minimum number of atoms generated
  // for a string using *nocase*, *ascii* and *wide* modifiers simultaneously.

  if (rule->num_atoms > YR_ATOMS_PER_RULE_WARNING_THRESHOLD)
  {
    yywarning(yyscanner, "rule is slowing down scanning");
  }

  yr_rule_strings_foreach(rule, string)
  {
    // Only the heading fragment in a chain of strings (the one with
    // chained_to == NULL) must be referenced. All other fragments
    // are never marked as referenced.

    if (!STRING_IS_REFERENCED(string) && string->chained_to == NULL)
    {
      yr_compiler_set_error_extra_info(
          compiler, string->identifier) return ERROR_UNREFERENCED_STRING;
    }

    strings_in_rule++;

    if (strings_in_rule > max_strings_per_rule)
    {
      yr_compiler_set_error_extra_info(
          compiler, rule->identifier) return ERROR_TOO_MANY_STRINGS;
    }
  }

  FAIL_ON_ERROR(yr_parser_emit_with_arg(
      yyscanner, OP_MATCH_RULE, compiler->current_rule_idx, NULL, NULL));

  fixup = compiler->fixup_stack_head;

  int32_t* jmp_offset_addr = (int32_t*) yr_arena_ref_to_ptr(
      compiler->arena, &fixup->ref);

  int32_t jmp_offset = yr_arena_get_current_offset(
                           compiler->arena, YR_CODE_SECTION) -
                       fixup->ref.offset + 1;

  *jmp_offset_addr = jmp_offset;

  // Remove fixup from the stack.
  compiler->fixup_stack_head = fixup->next;
  yr_free(fixup);

  // We have finished parsing the current rule set current_rule_idx to
  // UINT32_MAX indicating that we are not currently parsing a rule.
  compiler->current_rule_idx = UINT32_MAX;

  return ERROR_SUCCESS;
}