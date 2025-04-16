int yr_scan_verify_match(
    YR_SCAN_CONTEXT* context,
    YR_AC_MATCH* ac_match,
    const uint8_t* data,
    size_t data_size,
    uint64_t data_base,
    size_t offset)
{
  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "- %s(data=%p data_size=%zu data_base=0x%" PRIx64 " offset=%zu) {}\n",
      __FUNCTION__,
      data,
      data_size,
      data_base,
      offset);

  YR_STRING* string = ac_match->string;
  YR_CALLBACK_FUNC callback = context->callback;

  int result;

  if (data_size - offset <= 0)
    return ERROR_SUCCESS;

  if (yr_bitmask_is_set(context->strings_temp_disabled, string->idx))
    return ERROR_SUCCESS;

  if (context->flags & SCAN_FLAGS_FAST_MODE && STRING_IS_SINGLE_MATCH(string) &&
      context->matches[string->idx].head != NULL)
    return ERROR_SUCCESS;

  if (STRING_IS_FIXED_OFFSET(string) &&
      string->fixed_offset != data_base + offset)
    return ERROR_SUCCESS;

#ifdef YR_PROFILING_ENABLED
  uint64_t start_time;
  bool sample = context->profiling_info[string->rule_idx].atom_matches %
                    YR_MATCH_VERIFICATION_PROFILING_RATE ==
                0;

  if (sample)
    start_time = yr_stopwatch_elapsed_ns(&context->stopwatch);
#endif

  if (STRING_IS_LITERAL(string))
  {
    result = _yr_scan_verify_literal_match(
        context, ac_match, data, data_size, data_base, offset);
  }
  else
  {
    result = _yr_scan_verify_re_match(
        context, ac_match, data, data_size, data_base, offset);
  }

  // If _yr_scan_verify_literal_match or _yr_scan_verify_re_match return
  // ERROR_TOO_MANY_MATCHES call the callback with CALLBACK_MSG_TOO_MANY_MATCHES
  // in order to ask what to do. If the callback returns CALLBACK_CONTINUE
  // this error is ignored, if not, the error is propagated to the caller.
  if (result == ERROR_TOO_MANY_MATCHES)
  {
    result = callback(
        context,
        CALLBACK_MSG_TOO_MANY_MATCHES,
        (void*) string,
        context->user_data);

    switch (result)
    {
    case CALLBACK_CONTINUE:
      yr_bitmask_set(context->strings_temp_disabled, string->idx);
      result = ERROR_SUCCESS;
      break;

    default:
      result = ERROR_TOO_MANY_MATCHES;
      break;
    }
  }

#ifdef YR_PROFILING_ENABLED
  if (sample)
  {
    uint64_t finish_time = yr_stopwatch_elapsed_ns(&context->stopwatch);
    context->profiling_info[string->rule_idx].match_time +=
        (finish_time - start_time);
  }
  context->profiling_info[string->rule_idx].atom_matches++;
#endif

  if (result != ERROR_SUCCESS)
    context->last_error_string = string;

  return result;
}