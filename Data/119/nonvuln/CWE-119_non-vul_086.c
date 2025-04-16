static struct mschmd_header *chmd_open(struct mschm_decompressor *base,
                                       const char *filename)
{
  return chmd_real_open(base, filename, 1);
}