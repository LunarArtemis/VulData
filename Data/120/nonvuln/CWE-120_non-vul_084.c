int _yr_compiler_push_file_name(YR_COMPILER* compiler, const char* file_name)
{
  char* str;
  int i;

  for (i = 0; i < compiler->file_name_stack_ptr; i++)
  {
    if (strcmp(file_name, compiler->file_name_stack[i]) == 0)
      return ERROR_INCLUDES_CIRCULAR_REFERENCE;
  }

  if (compiler->file_name_stack_ptr == YR_MAX_INCLUDE_DEPTH)
    return ERROR_INCLUDE_DEPTH_EXCEEDED;

  str = yr_strdup(file_name);

  if (str == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  compiler->file_name_stack[compiler->file_name_stack_ptr] = str;
  compiler->file_name_stack_ptr++;

  return ERROR_SUCCESS;
}