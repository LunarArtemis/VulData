int _yr_compiler_get_var_frame(YR_COMPILER* compiler)
{
  int i, result = 0;

  for (i = 0; i < compiler->loop_index; i++)
  {
    result += compiler->loop[i].vars_count +
              compiler->loop[i].vars_internal_count;
  }

  return result;
}