static int _yr_scan_xor_wcompare(
    const uint8_t* data,
    size_t data_size,
    uint8_t* string,
    size_t string_length)
{
  const uint8_t* s1 = data;
  const uint8_t* s2 = string;
  uint8_t k = 0;

  size_t i = 0;

  if (data_size < string_length * 2)
    return 0;

  // Calculate the xor key to compare with. *s1 is the start of the string we
  // matched on and *s2 is the "plaintext" string, so *s1 ^ *s2 is the key to
  // every *s2 as we compare.
  k = *s1 ^ *s2;

  while (i < string_length && *s1 == ((*s2) ^ k) && ((*(s1 + 1)) ^ k) == 0x00)
  {
    s1 += 2;
    s2++;
    i++;
  }

  return (int) ((i == string_length) ? i * 2 : 0);
}