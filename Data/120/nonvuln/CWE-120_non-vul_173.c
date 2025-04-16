static int _yr_scan_xor_compare(
    const uint8_t* data,
    size_t data_size,
    uint8_t* string,
    size_t string_length)
{
  int result = 0;
  const uint8_t* s1 = data;
  const uint8_t* s2 = string;
  uint8_t k = 0;

  size_t i = 0;

  if (data_size < string_length)
    goto _exit;

  // Calculate the xor key to compare with. *s1 is the start of the string we
  // matched on and *s2 is the "plaintext" string, so *s1 ^ *s2 is the key to
  // every *s2 as we compare.
  k = *s1 ^ *s2;

  while (i < string_length && *s1++ == ((*s2++) ^ k)) i++;

  result = (int) ((i == string_length) ? i : 0);

_exit:;

  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "- %s(data_size=%zu string_length=%zu) {} = %d\n",
      __FUNCTION__,
      data_size,
      string_length,
      result);

  return result;
}