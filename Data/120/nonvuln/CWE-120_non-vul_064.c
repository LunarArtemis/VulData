static int _yr_parser_operator_to_opcode(const char* op, int expression_type)
{
  int opcode = 0;

  switch (expression_type)
  {
  case EXPRESSION_TYPE_INTEGER:
    opcode = OP_INT_BEGIN;
    break;
  case EXPRESSION_TYPE_FLOAT:
    opcode = OP_DBL_BEGIN;
    break;
  case EXPRESSION_TYPE_STRING:
    opcode = OP_STR_BEGIN;
    break;
  default:
    assert(false);
  }

  if (op[0] == '<')
  {
    if (op[1] == '=')
      opcode += _OP_LE;
    else
      opcode += _OP_LT;
  }
  else if (op[0] == '>')
  {
    if (op[1] == '=')
      opcode += _OP_GE;
    else
      opcode += _OP_GT;
  }
  else if (op[1] == '=')
  {
    if (op[0] == '=')
      opcode += _OP_EQ;
    else
      opcode += _OP_NEQ;
  }
  else if (op[0] == '+')
  {
    opcode += _OP_ADD;
  }
  else if (op[0] == '-')
  {
    opcode += _OP_SUB;
  }
  else if (op[0] == '*')
  {
    opcode += _OP_MUL;
  }
  else if (op[0] == '\\')
  {
    opcode += _OP_DIV;
  }

  if (IS_INT_OP(opcode) || IS_DBL_OP(opcode) || IS_STR_OP(opcode))
  {
    return opcode;
  }

  return OP_ERROR;
}