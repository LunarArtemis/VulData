static int _yr_scan_verify_literal_match(
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

  int flags = 0;
  int forward_matches = 0;

  CALLBACK_ARGS callback_args;
  YR_STRING* string = ac_match->string;

  if (STRING_FITS_IN_ATOM(string))
  {
    forward_matches = ac_match->backtrack;
  }
  else if (STRING_IS_NO_CASE(string))
  {
    if (STRING_IS_ASCII(string))
    {
      forward_matches = _yr_scan_icompare(
          data + offset, data_size - offset, string->string, string->length);
    }

    if (STRING_IS_WIDE(string) && forward_matches == 0)
    {
      forward_matches = _yr_scan_wicompare(
          data + offset, data_size - offset, string->string, string->length);
    }
  }
  else
  {
    if (STRING_IS_ASCII(string))
    {
      forward_matches = _yr_scan_compare(
          data + offset, data_size - offset, string->string, string->length);
    }

    if (STRING_IS_WIDE(string) && forward_matches == 0)
    {
      forward_matches = _yr_scan_wcompare(
          data + offset, data_size - offset, string->string, string->length);
    }

    if (STRING_IS_XOR(string) && forward_matches == 0)
    {
      if (STRING_IS_WIDE(string))
      {
        forward_matches = _yr_scan_xor_wcompare(
            data + offset, data_size - offset, string->string, string->length);
      }

      if (forward_matches == 0)
      {
        forward_matches = _yr_scan_xor_compare(
            data + offset, data_size - offset, string->string, string->length);
      }
    }
  }

  if (forward_matches == 0)
    return ERROR_SUCCESS;

  if (forward_matches == string->length * 2)
    flags |= RE_FLAGS_WIDE;

  if (STRING_IS_NO_CASE(string))
    flags |= RE_FLAGS_NO_CASE;

  callback_args.context = context;
  callback_args.string = string;
  callback_args.data = data;
  callback_args.data_size = data_size;
  callback_args.data_base = data_base;
  callback_args.forward_matches = forward_matches;
  callback_args.full_word = STRING_IS_FULL_WORD(string);

  FAIL_ON_ERROR(
      _yr_scan_match_callback(data + offset, 0, flags, &callback_args));

  return ERROR_SUCCESS;
}