YR_API void yr_compiler_set_include_callback(
    YR_COMPILER* compiler,
    YR_COMPILER_INCLUDE_CALLBACK_FUNC include_callback,
    YR_COMPILER_INCLUDE_FREE_FUNC include_free,
    void* user_data)
{
  compiler->include_callback = include_callback;
  compiler->include_free = include_free;
  compiler->incl_clbk_user_data = user_data;
}