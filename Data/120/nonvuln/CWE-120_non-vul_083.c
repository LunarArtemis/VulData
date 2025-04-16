YR_API int yr_compiler_create(YR_COMPILER** compiler)
{
  int result;
  YR_COMPILER* new_compiler;

  new_compiler = (YR_COMPILER*) yr_calloc(1, sizeof(YR_COMPILER));

  if (new_compiler == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  new_compiler->current_rule_idx = UINT32_MAX;
  new_compiler->next_rule_idx = 0;
  new_compiler->current_string_idx = 0;
  new_compiler->current_namespace_idx = 0;
  new_compiler->current_meta_idx = 0;
  new_compiler->num_namespaces = 0;
  new_compiler->errors = 0;
  new_compiler->callback = NULL;
  new_compiler->rules = NULL;
  new_compiler->include_callback = _yr_compiler_default_include_callback;
  new_compiler->incl_clbk_user_data = NULL;
  new_compiler->include_free = _yr_compiler_default_include_free;
  new_compiler->re_ast_callback = NULL;
  new_compiler->re_ast_clbk_user_data = NULL;
  new_compiler->last_error = ERROR_SUCCESS;
  new_compiler->last_error_line = 0;
  new_compiler->current_line = 0;
  new_compiler->file_name_stack_ptr = 0;
  new_compiler->fixup_stack_head = NULL;
  new_compiler->loop_index = -1;
  new_compiler->loop_for_of_var_index = -1;

  new_compiler->atoms_config.get_atom_quality = yr_atoms_heuristic_quality;
  new_compiler->atoms_config.quality_warning_threshold =
      YR_ATOM_QUALITY_WARNING_THRESHOLD;

  result = yr_hash_table_create(5000, &new_compiler->rules_table);

  if (result == ERROR_SUCCESS)
    result = yr_hash_table_create(1000, &new_compiler->objects_table);

  if (result == ERROR_SUCCESS)
    result = yr_hash_table_create(10000, &new_compiler->strings_table);

  if (result == ERROR_SUCCESS)
    result = yr_hash_table_create(10000, &new_compiler->sz_table);

  if (result == ERROR_SUCCESS)
    result = yr_arena_create(YR_NUM_SECTIONS, 1048576, &new_compiler->arena);

  if (result == ERROR_SUCCESS)
    result = yr_ac_automaton_create(
        new_compiler->arena, &new_compiler->automaton);

  if (result == ERROR_SUCCESS)
  {
    *compiler = new_compiler;
  }
  else  // if error, do cleanup
  {
    yr_compiler_destroy(new_compiler);
  }

  return result;
}