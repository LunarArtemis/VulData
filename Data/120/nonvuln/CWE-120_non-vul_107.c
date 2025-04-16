void _yr_compiler_pop_file_name(YR_COMPILER* compiler)
{
  if (compiler->file_name_stack_ptr > 0)
  {
    compiler->file_name_stack_ptr--;
    yr_free(compiler->file_name_stack[compiler->file_name_stack_ptr]);
    compiler->file_name_stack[compiler->file_name_stack_ptr] = NULL;
  }
}