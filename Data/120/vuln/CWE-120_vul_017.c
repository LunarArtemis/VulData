static int _yr_scan_match_callback(
    const uint8_t* match_data,
    int32_t match_length,
    int flags,
    void* args)
{
  CALLBACK_ARGS* callback_args = (CALLBACK_ARGS*) args;

  YR_STRING* string = callback_args->string;
  YR_MATCH* new_match;

  int result = ERROR_SUCCESS;

  size_t match_offset = match_data - callback_args->data;

  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "+ %s(match_data=%p match_length=%d) { //"
      " match_offset=%zu args->data=%p args->string.length=%u"
      " args->data_base=0x%" PRIx64 " args->data_size=%zu"
      " args->forward_matches=%'u\n",
      __FUNCTION__,
      match_data,
      match_length,
      match_offset,
      callback_args->data,
      callback_args->string->length,
      callback_args->data_base,
      callback_args->data_size,
      callback_args->forward_matches);

  // total match length is the sum of backward and forward matches.
  match_length += callback_args->forward_matches;

  // make sure that match fits into the data.
  assert(match_offset + match_length <= callback_args->data_size);

  if (callback_args->full_word)
  {
    if (flags & RE_FLAGS_WIDE)
    {
      if (match_offset >= 2 && *(match_data - 1) == 0 &&
          yr_isalnum(match_data - 2))
        goto _exit;  // return ERROR_SUCCESS;

      if (match_offset + match_length + 1 < callback_args->data_size &&
          *(match_data + match_length + 1) == 0 &&
          yr_isalnum(match_data + match_length))
        goto _exit;  // return ERROR_SUCCESS;
    }
    else
    {
      if (match_offset >= 1 && yr_isalnum(match_data - 1))
        goto _exit;  // return ERROR_SUCCESS;

      if (match_offset + match_length < callback_args->data_size &&
          yr_isalnum(match_data + match_length))
        goto _exit;  // return ERROR_SUCCESS;
    }
  }

  if (STRING_IS_CHAIN_PART(string))
  {
    result = _yr_scan_verify_chained_string_match(
        string,
        callback_args->context,
        match_data,
        callback_args->data_base,
        match_offset,
        match_length);
  }
  else
  {
    uint32_t max_match_data;

    FAIL_ON_ERROR(
        yr_get_configuration(YR_CONFIG_MAX_MATCH_DATA, &max_match_data));

    new_match = yr_notebook_alloc(
        callback_args->context->matches_notebook, sizeof(YR_MATCH));

    if (new_match == NULL)
    {
      result = ERROR_INSUFFICIENT_MEMORY;
      goto _exit;
    }

    new_match->data_length = yr_min(match_length, (int32_t) max_match_data);

    if (new_match->data_length > 0)
    {
      new_match->data = yr_notebook_alloc(
          callback_args->context->matches_notebook, new_match->data_length);

      if (new_match->data == NULL)
      {
        result = ERROR_INSUFFICIENT_MEMORY;
        goto _exit;
      }

      memcpy((void*) new_match->data, match_data, new_match->data_length);
    }
    else
    {
      new_match->data = NULL;
    }

    if (result == ERROR_SUCCESS)
    {
      new_match->base = callback_args->data_base;
      new_match->offset = match_offset;
      new_match->match_length = match_length;
      new_match->prev = NULL;
      new_match->next = NULL;
      new_match->is_private = STRING_IS_PRIVATE(string);

      FAIL_ON_ERROR(_yr_scan_add_match_to_list(
          new_match,
          &callback_args->context->matches[string->idx],
          STRING_IS_GREEDY_REGEXP(string)));
    }
  }

_exit:;

  YR_DEBUG_FPRINTF(2, stderr, "} = %d // %s()\n", result, __FUNCTION__);

  return result;
}