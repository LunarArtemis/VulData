YR_API void yr_scanner_destroy(YR_SCANNER* scanner)
{
  YR_DEBUG_FPRINTF(2, stderr, "- %s() {} \n", __FUNCTION__);

  RE_FIBER* fiber = scanner->re_fiber_pool.fibers.head;

  while (fiber != NULL)
  {
    RE_FIBER* next = fiber->next;
    yr_free(fiber);
    fiber = next;
  }

  RE_FAST_EXEC_POSITION* position = scanner->re_fast_exec_position_pool.head;

  while (position != NULL)
  {
    RE_FAST_EXEC_POSITION* next = position->next;
    yr_free(position);
    position = next;
  }

  if (scanner->objects_table != NULL)
  {
    yr_hash_table_destroy(
        scanner->objects_table,
        (YR_HASH_TABLE_FREE_VALUE_FUNC) yr_object_destroy);
  }

#ifdef YR_PROFILING_ENABLED
  yr_free(scanner->profiling_info);
#endif

  yr_free(scanner->rule_matches_flags);
  yr_free(scanner->ns_unsatisfied_flags);
  yr_free(scanner->strings_temp_disabled);
  yr_free(scanner->matches);
  yr_free(scanner->unconfirmed_matches);
  yr_free(scanner);
}