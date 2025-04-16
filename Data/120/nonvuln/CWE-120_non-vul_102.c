YR_API char* yr_compiler_get_current_file_name(YR_COMPILER* compiler)
{
  if (compiler->file_name_stack_ptr > 0)
  {
    return compiler->file_name_stack[compiler->file_name_stack_ptr - 1];
  }
  else
  {
    return NULL;
  }
}