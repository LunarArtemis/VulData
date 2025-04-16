static void _yr_compiler_default_include_free(
    const char* callback_result_ptr,
    void* user_data)
{
  if (callback_result_ptr != NULL)
  {
    yr_free((void*) callback_result_ptr);
  }
}