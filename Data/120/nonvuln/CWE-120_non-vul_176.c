static int _yr_scan_verify_re_match(
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

  CALLBACK_ARGS callback_args;
  RE_EXEC_FUNC exec;

  int forward_matches = -1;
  int backward_matches = -1;
  int flags = 0;

  if (STRING_IS_GREEDY_REGEXP(ac_match->string))
    flags |= RE_FLAGS_GREEDY;

  if (STRING_IS_NO_CASE(ac_match->string))
    flags |= RE_FLAGS_NO_CASE;

  if (STRING_IS_DOT_ALL(ac_match->string))
    flags |= RE_FLAGS_DOT_ALL;

  if (STRING_IS_FAST_REGEXP(ac_match->string))
    exec = yr_re_fast_exec;
  else
    exec = yr_re_exec;

  if (STRING_IS_ASCII(ac_match->string) || STRING_IS_BASE64(ac_match->string) ||
      STRING_IS_BASE64_WIDE(ac_match->string))
  {
    FAIL_ON_ERROR(exec(
        context,
        ac_match->forward_code,
        data + offset,
        data_size - offset,
        offset,
        flags,
        NULL,
        NULL,
        &forward_matches));
  }

  if ((forward_matches == -1) && (STRING_IS_WIDE(ac_match->string) &&
                                  !(STRING_IS_BASE64(ac_match->string) ||
                                    STRING_IS_BASE64_WIDE(ac_match->string))))
  {
    flags |= RE_FLAGS_WIDE;
    FAIL_ON_ERROR(exec(
        context,
        ac_match->forward_code,
        data + offset,
        data_size - offset,
        offset,
        flags,
        NULL,
        NULL,
        &forward_matches));
  }

  if (forward_matches == -1)
    return ERROR_SUCCESS;

  if (forward_matches == 0 && ac_match->backward_code == NULL)
    return ERROR_SUCCESS;

  callback_args.string = ac_match->string;
  callback_args.context = context;
  callback_args.data = data;
  callback_args.data_size = data_size;
  callback_args.data_base = data_base;
  callback_args.forward_matches = forward_matches;
  callback_args.full_word = STRING_IS_FULL_WORD(ac_match->string);

  if (ac_match->backward_code != NULL)
  {
    FAIL_ON_ERROR(exec(
        context,
        ac_match->backward_code,
        data + offset,
        data_size - offset,
        offset,
        flags | RE_FLAGS_BACKWARDS | RE_FLAGS_EXHAUSTIVE,
        _yr_scan_match_callback,
        (void*) &callback_args,
        &backward_matches));
  }
  else
  {
    FAIL_ON_ERROR(
        _yr_scan_match_callback(data + offset, 0, flags, &callback_args));
  }

  return ERROR_SUCCESS;
}