static int hash_fn(const char *key, int table_size)
{
  unsigned int hashed = 0;
  const char *c;
  int bits_per_int = sizeof(unsigned int)*8;

  for (c = key; *c; c++) {
    /* letters have about 5 bits in them */
    hashed = (hashed<<5) | (hashed>>(bits_per_int-5));
    hashed ^= *c;
  }
  return hashed % table_size;
}