YR_API void yr_compiler_set_callback(
    YR_COMPILER* compiler,
    YR_COMPILER_CALLBACK_FUNC callback,
    void* user_data)
{
  compiler->callback = callback;
  compiler->user_data = user_data;
}