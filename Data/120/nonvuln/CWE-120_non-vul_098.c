int _yr_compiler_store_data(
    YR_COMPILER* compiler,
    const void* data,
    size_t data_length,
    YR_ARENA_REF* ref)
{
  // Check if the data is already in YR_SZ_POOL by using a hash table.
  uint32_t offset = yr_hash_table_lookup_uint32_raw_key(
      compiler->sz_table, data, data_length, NULL);

  if (offset == UINT32_MAX)
  {
    // The data was not previously written to YR_SZ_POOL, write it and store
    // the reference's offset in the hash table. Storing the buffer number
    // is not necessary, it's always YR_SZ_POOL.
    FAIL_ON_ERROR(yr_arena_write_data(
        compiler->arena, YR_SZ_POOL, data, data_length, ref));

    FAIL_ON_ERROR(yr_hash_table_add_uint32_raw_key(
        compiler->sz_table, data, data_length, NULL, ref->offset));
  }
  else
  {
    ref->buffer_id = YR_SZ_POOL;
    ref->offset = offset;
  }

  return ERROR_SUCCESS;
}