int yr_parser_reduce_operation(
    yyscan_t yyscanner,
    const char* op,
    YR_EXPRESSION left_operand,
    YR_EXPRESSION right_operand)
{
  int expression_type;

  YR_COMPILER* compiler = yyget_extra(yyscanner);

  if ((left_operand.type == EXPRESSION_TYPE_INTEGER ||
       left_operand.type == EXPRESSION_TYPE_FLOAT) &&
      (right_operand.type == EXPRESSION_TYPE_INTEGER ||
       right_operand.type == EXPRESSION_TYPE_FLOAT))
  {
    if (left_operand.type != right_operand.type)
    {
      // One operand is double and the other is integer,
      // cast the integer to double

      FAIL_ON_ERROR(yr_parser_emit_with_arg(
          yyscanner,
          OP_INT_TO_DBL,
          (left_operand.type == EXPRESSION_TYPE_INTEGER) ? 2 : 1,
          NULL,
          NULL));
    }

    expression_type = EXPRESSION_TYPE_FLOAT;

    if (left_operand.type == EXPRESSION_TYPE_INTEGER &&
        right_operand.type == EXPRESSION_TYPE_INTEGER)
    {
      expression_type = EXPRESSION_TYPE_INTEGER;
    }

    FAIL_ON_ERROR(yr_parser_emit(
        yyscanner, _yr_parser_operator_to_opcode(op, expression_type), NULL));
  }
  else if (
      left_operand.type == EXPRESSION_TYPE_STRING &&
      right_operand.type == EXPRESSION_TYPE_STRING)
  {
    int opcode = _yr_parser_operator_to_opcode(op, EXPRESSION_TYPE_STRING);

    if (opcode != OP_ERROR)
    {
      FAIL_ON_ERROR(yr_parser_emit(yyscanner, opcode, NULL));
    }
    else
    {
      yr_compiler_set_error_extra_info_fmt(
          compiler, "strings don't support \"%s\" operation", op);

      return ERROR_WRONG_TYPE;
    }
  }
  else
  {
    yr_compiler_set_error_extra_info(compiler, "type mismatch");

    return ERROR_WRONG_TYPE;
  }

  return ERROR_SUCCESS;
}