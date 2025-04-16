static void print_compiler_error(
    int error_level,
    const char* file_name,
    int line_number,
    const YR_RULE* rule,
    const char* message,
    void* user_data)
{
  char* msg_type;

  if (error_level == YARA_ERROR_LEVEL_ERROR)
  {
    msg_type = "error";
  }
  else if (!ignore_warnings)
  {
    COMPILER_RESULTS* compiler_results = (COMPILER_RESULTS*) user_data;
    compiler_results->warnings++;
    msg_type = "warning";
  }
  else
  {
    return;
  }

  if (rule != NULL)
  {
    fprintf(
        stderr,
        "%s: rule \"%s\" in %s(%d): %s\n",
        msg_type,
        rule->identifier,
        file_name,
        line_number,
        message);
  }
  else
  {
    fprintf(
        stderr, "%s(%d): %s: %s\n", file_name, line_number, msg_type, message);
  }
}