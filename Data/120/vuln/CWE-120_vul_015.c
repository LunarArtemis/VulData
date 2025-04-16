YR_API int yr_scanner_scan_mem_blocks(
    YR_SCANNER* scanner,
    YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  YR_DEBUG_FPRINTF(2, stderr, "+ %s() {\n", __FUNCTION__);

  YR_RULES* rules;
  YR_RULE* rule;
  YR_MEMORY_BLOCK* block;

  int i, result = ERROR_SUCCESS;

  if (scanner->callback == NULL)
  {
    result = ERROR_CALLBACK_REQUIRED;
    goto _exit;
  }

  scanner->iterator = iterator;
  rules = scanner->rules;

  if (iterator->last_error == ERROR_BLOCK_NOT_READY)
  {
    // The caller is invoking yr_scanner_scan_mem_blocks again because the
    // previous call returned ERROR_BLOCK_NOT_READY.
    block = iterator->next(iterator);
  }
  else
  {
    // Create the notebook that will hold the YR_MATCH structures representing
    // each match found. This notebook will also contain snippets of the
    // matching data (the "data" field in YR_MATCH points to the snippet
    // corresponding to the match). Each notebook's page can store up to 1024
    // matches.
    uint32_t max_match_data;

    FAIL_ON_ERROR(
        yr_get_configuration(YR_CONFIG_MAX_MATCH_DATA, &max_match_data));

    result = yr_notebook_create(
        1024 * (sizeof(YR_MATCH) + max_match_data), &scanner->matches_notebook);

    if (result != ERROR_SUCCESS)
      goto _exit;

    yr_stopwatch_start(&scanner->stopwatch);

    block = iterator->first(iterator);
  }

  while (block != NULL)
  {
    const uint8_t* data = block->fetch_data(block);

    // fetch_data may fail and return NULL.
    if (data == NULL)
    {
      block = iterator->next(iterator);
      continue;
    }

    if (scanner->entry_point == YR_UNDEFINED)
    {
      YR_TRYCATCH(
          !(scanner->flags & SCAN_FLAGS_NO_TRYCATCH),
          {
            if (scanner->flags & SCAN_FLAGS_PROCESS_MEMORY)
              scanner->entry_point = yr_get_entry_point_address(
                  data, block->size, block->base);
            else
              scanner->entry_point = yr_get_entry_point_offset(
                  data, block->size);
          },
          {});
    }

    YR_TRYCATCH(
        !(scanner->flags & SCAN_FLAGS_NO_TRYCATCH),
        { result = _yr_scanner_scan_mem_block(scanner, data, block); }