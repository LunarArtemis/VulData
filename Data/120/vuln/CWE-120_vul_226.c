void* __iotc_calloc(size_t num, size_t byte_count) {
  const size_t size_to_allocate = num * byte_count;
  void* ret = iotc_bsp_mem_alloc(size_to_allocate);

  /* It's unspecified if memset works with NULL pointer. */
  if (NULL != ret) {
    memset(ret, 0, size_to_allocate);
  }

  return ret;
}