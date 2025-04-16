YR_API void yr_compiler_set_re_ast_callback(
    YR_COMPILER* compiler,
    YR_COMPILER_RE_AST_CALLBACK_FUNC re_ast_callback,
    void* user_data)
{
  compiler->re_ast_callback = re_ast_callback;
  compiler->re_ast_clbk_user_data = user_data;
}