YR_API int yr_scanner_create(YR_RULES* rules, YR_SCANNER** scanner)
{
  YR_DEBUG_FPRINTF(2, stderr, "- %s() {} \n", __FUNCTION__);

  YR_EXTERNAL_VARIABLE* external;
  YR_SCANNER* new_scanner;

  new_scanner = (YR_SCANNER*) yr_calloc(1, sizeof(YR_SCANNER));

  if (new_scanner == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  FAIL_ON_ERROR_WITH_CLEANUP(
      yr_hash_table_create(64, &new_scanner->objects_table),
      yr_free(new_scanner));

  new_scanner->rules = rules;
  new_scanner->entry_point = YR_UNDEFINED;
  new_scanner->file_size = YR_UNDEFINED;
  new_scanner->canary = rand();

  // By default report both matching and non-matching rules.
  new_scanner->flags = SCAN_FLAGS_REPORT_RULES_MATCHING |
                       SCAN_FLAGS_REPORT_RULES_NOT_MATCHING;

  new_scanner->rule_matches_flags = (YR_BITMASK*) yr_calloc(
      sizeof(YR_BITMASK), YR_BITMASK_SIZE(rules->num_rules));

  new_scanner->ns_unsatisfied_flags = (YR_BITMASK*) yr_calloc(
      sizeof(YR_BITMASK), YR_BITMASK_SIZE(rules->num_namespaces));

  new_scanner->strings_temp_disabled = (YR_BITMASK*) yr_calloc(
      sizeof(YR_BITMASK), YR_BITMASK_SIZE(rules->num_strings));

  new_scanner->matches = (YR_MATCHES*) yr_calloc(
      rules->num_strings, sizeof(YR_MATCHES));

  new_scanner->unconfirmed_matches = (YR_MATCHES*) yr_calloc(
      rules->num_strings, sizeof(YR_MATCHES));

#ifdef YR_PROFILING_ENABLED
  new_scanner->profiling_info = yr_calloc(
      rules->num_rules, sizeof(YR_PROFILING_INFO));

  if (new_scanner->profiling_info == NULL)
  {
    yr_scanner_destroy(new_scanner);
    return ERROR_INSUFFICIENT_MEMORY;
  }
#else
  new_scanner->profiling_info = NULL;
#endif

  external = rules->ext_vars_table;

  while (!EXTERNAL_VARIABLE_IS_NULL(external))
  {
    YR_OBJECT* object;

    FAIL_ON_ERROR_WITH_CLEANUP(
        yr_object_from_external_variable(external, &object),
        // cleanup
        yr_scanner_destroy(new_scanner));

    FAIL_ON_ERROR_WITH_CLEANUP(yr_hash_table_add(
                                   new_scanner->objects_table,
                                   external->identifier,
                                   NULL,
                                   (void*) object),
                               // cleanup
                               yr_object_destroy(object);
                               yr_scanner_destroy(new_scanner));

    yr_object_set_canary(object, new_scanner->canary);
    external++;
  }

  *scanner = new_scanner;

  return ERROR_SUCCESS;
}