int _yr_compiler_store_string(
    YR_COMPILER* compiler,
    const char* string,
    YR_ARENA_REF* ref)
{
  return _yr_compiler_store_data(
      compiler,
      (void*) string,
      strlen(string) + 1,  // include the null terminator
      ref);
}