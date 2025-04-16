YR_API void yr_compiler_destroy(YR_COMPILER* compiler)
{
  yr_arena_release(compiler->arena);

  if (compiler->automaton != NULL)
    yr_ac_automaton_destroy(compiler->automaton);

  yr_hash_table_destroy(compiler->rules_table, NULL);

  yr_hash_table_destroy(compiler->strings_table, NULL);

  yr_hash_table_destroy(compiler->sz_table, NULL);

  yr_hash_table_destroy(
      compiler->objects_table,
      (YR_HASH_TABLE_FREE_VALUE_FUNC) yr_object_destroy);

  if (compiler->atoms_config.free_quality_table)
    yr_free(compiler->atoms_config.quality_table);

  for (int i = 0; i < compiler->file_name_stack_ptr; i++)
    yr_free(compiler->file_name_stack[i]);

  YR_FIXUP* fixup = compiler->fixup_stack_head;

  while (fixup != NULL)
  {
    YR_FIXUP* next_fixup = fixup->next;
    yr_free(fixup);
    fixup = next_fixup;
  }

  yr_free(compiler);
}