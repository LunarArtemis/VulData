static int _yr_parser_check_string_modifiers(
    yyscan_t yyscanner,
    YR_MODIFIER modifier)
{
  YR_COMPILER* compiler = yyget_extra(yyscanner);

  // xor and nocase together is not implemented.
  if (modifier.flags & STRING_FLAGS_XOR &&
      modifier.flags & STRING_FLAGS_NO_CASE)
  {
    yr_compiler_set_error_extra_info(
        compiler, "invalid modifier combination: xor nocase");
    return ERROR_INVALID_MODIFIER;
  }

  // base64 and nocase together is not implemented.
  if (modifier.flags & STRING_FLAGS_NO_CASE &&
      (modifier.flags & STRING_FLAGS_BASE64 ||
       modifier.flags & STRING_FLAGS_BASE64_WIDE))
  {
    yr_compiler_set_error_extra_info(
        compiler,
        modifier.flags & STRING_FLAGS_BASE64
            ? "invalid modifier combination: base64 nocase"
            : "invalid modifier combination: base64wide nocase");
    return ERROR_INVALID_MODIFIER;
  }

  // base64 and fullword together is not implemented.
  if (modifier.flags & STRING_FLAGS_FULL_WORD &&
      (modifier.flags & STRING_FLAGS_BASE64 ||
       modifier.flags & STRING_FLAGS_BASE64_WIDE))
  {
    yr_compiler_set_error_extra_info(
        compiler,
        modifier.flags & STRING_FLAGS_BASE64
            ? "invalid modifier combination: base64 fullword"
            : "invalid modifier combination: base64wide fullword");
    return ERROR_INVALID_MODIFIER;
  }

  // base64 and xor together is not implemented.
  if (modifier.flags & STRING_FLAGS_XOR &&
      (modifier.flags & STRING_FLAGS_BASE64 ||
       modifier.flags & STRING_FLAGS_BASE64_WIDE))
  {
    yr_compiler_set_error_extra_info(
        compiler,
        modifier.flags & STRING_FLAGS_BASE64
            ? "invalid modifier combination: base64 xor"
            : "invalid modifier combination: base64wide xor");
    return ERROR_INVALID_MODIFIER;
  }

  return ERROR_SUCCESS;
}