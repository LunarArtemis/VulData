static uint64_t _yr_get_file_size(YR_MEMORY_BLOCK_ITERATOR* iterator)
{
  uint64_t file_size = ((YR_MEMORY_BLOCK*) iterator->context)->size;

  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "- %s() {} = %" PRIu64
      "  // default iterator; single memory block, blocking\n",
      __FUNCTION__,
      file_size);

  return file_size;
}