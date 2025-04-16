int yr_parser_lookup_loop_variable(
    yyscan_t yyscanner,
    const char* identifier,
    YR_EXPRESSION* expr)
{
  YR_COMPILER* compiler = yyget_extra(yyscanner);
  int i, j;
  int var_offset = 0;

  for (i = 0; i <= compiler->loop_index; i++)
  {
    var_offset += compiler->loop[i].vars_internal_count;

    for (j = 0; j < compiler->loop[i].vars_count; j++)
    {
      if (compiler->loop[i].vars[j].identifier.ptr != NULL &&
          strcmp(identifier, compiler->loop[i].vars[j].identifier.ptr) == 0)
      {
        if (expr != NULL)
          *expr = compiler->loop[i].vars[j];

        return var_offset + j;
      }
    }

    var_offset += compiler->loop[i].vars_count;
  }

  return -1;
}