static int _yr_scan_wicompare(
    const uint8_t* data,
    size_t data_size,
    uint8_t* string,
    size_t string_length)
{
  int result = 0;
  const uint8_t* s1 = data;
  const uint8_t* s2 = string;

  size_t i = 0;

  if (data_size < string_length * 2)
    goto _exit;

  while (i < string_length && yr_lowercase[*s1] == yr_lowercase[*s2] &&
         *(s1 + 1) == 0x00)
  {
    s1 += 2;
    s2++;
    i++;
  }

  result = (int) ((i == string_length) ? i * 2 : 0);

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