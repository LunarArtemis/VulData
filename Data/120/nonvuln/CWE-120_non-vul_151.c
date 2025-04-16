YR_API int yr_scanner_scan_mem(
    YR_SCANNER* scanner,
    const uint8_t* buffer,
    size_t buffer_size)
{
  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "+ %s(buffer=%p buffer_size=%zu) {\n",
      __FUNCTION__,
      buffer,
      buffer_size);

  YR_MEMORY_BLOCK block;
  YR_MEMORY_BLOCK_ITERATOR iterator;

  block.size = buffer_size;
  block.base = 0;
  block.fetch_data = _yr_fetch_block_data;
  block.context = (void*) buffer;

  iterator.context = &block;
  iterator.first = _yr_get_first_block;
  iterator.next = _yr_get_next_block;
  iterator.file_size = _yr_get_file_size;
  iterator.last_error = ERROR_SUCCESS;

  int result = yr_scanner_scan_mem_blocks(scanner, &iterator);

  YR_DEBUG_FPRINTF(
      2,
      stderr,
      "} = %d AKA %s // %s()\n",
      result,
      yr_debug_error_as_string(result),
      __FUNCTION__);

  return result;
}