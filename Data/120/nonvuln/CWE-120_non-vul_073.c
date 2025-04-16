int yr_parser_emit_push_const(yyscan_t yyscanner, uint64_t argument)
{
  uint64_t u = (uint64_t) argument;
  uint8_t buf[9];
  int bufsz = 1;
  if (u == YR_UNDEFINED)
  {
    buf[0] = OP_PUSH_U;
  }
  else if (u <= 0xff)
  {
    buf[0] = OP_PUSH_8;
    bufsz += sizeof(uint8_t);
    buf[1] = (uint8_t) argument;
  }
  else if (u <= 0xffff)
  {
    buf[0] = OP_PUSH_16;
    bufsz += sizeof(uint16_t);
    *((uint16_t*) (buf + 1)) = (uint16_t) argument;
  }
  else if (u <= 0xffffffff)
  {
    buf[0] = OP_PUSH_32;
    bufsz += sizeof(uint32_t);
    *((uint32_t*) (buf + 1)) = (uint32_t) argument;
  }
  else
  {
    buf[0] = OP_PUSH;
    bufsz += sizeof(uint64_t);
    *((uint64_t*) (buf + 1)) = (uint64_t) argument;
  }
  return yr_arena_write_data(
      yyget_extra(yyscanner)->arena, YR_CODE_SECTION, buf, bufsz, NULL);
}