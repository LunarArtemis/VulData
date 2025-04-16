YR_API char* yr_compiler_get_error_message(
    YR_COMPILER* compiler,
    char* buffer,
    int buffer_size)
{
  uint32_t max_strings_per_rule;

  switch (compiler->last_error)
  {
  case ERROR_INSUFFICIENT_MEMORY:
    snprintf(buffer, buffer_size, "not enough memory");
    break;
  case ERROR_DUPLICATED_IDENTIFIER:
    snprintf(
        buffer,
        buffer_size,
        "duplicated identifier \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_DUPLICATED_STRING_IDENTIFIER:
    snprintf(
        buffer,
        buffer_size,
        "duplicated string identifier \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_DUPLICATED_TAG_IDENTIFIER:
    snprintf(
        buffer,
        buffer_size,
        "duplicated tag identifier \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_DUPLICATED_META_IDENTIFIER:
    snprintf(
        buffer,
        buffer_size,
        "duplicated metadata identifier \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_DUPLICATED_LOOP_IDENTIFIER:
    snprintf(
        buffer,
        buffer_size,
        "duplicated loop identifier \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_UNDEFINED_STRING:
    snprintf(
        buffer,
        buffer_size,
        "undefined string \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_UNDEFINED_IDENTIFIER:
    snprintf(
        buffer,
        buffer_size,
        "undefined identifier \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_UNREFERENCED_STRING:
    snprintf(
        buffer,
        buffer_size,
        "unreferenced string \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_EMPTY_STRING:
    snprintf(
        buffer,
        buffer_size,
        "empty string \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_NOT_A_STRUCTURE:
    snprintf(
        buffer,
        buffer_size,
        "\"%s\" is not a structure",
        compiler->last_error_extra_info);
    break;
  case ERROR_NOT_INDEXABLE:
    snprintf(
        buffer,
        buffer_size,
        "\"%s\" is not an array or dictionary",
        compiler->last_error_extra_info);
    break;
  case ERROR_NOT_A_FUNCTION:
    snprintf(
        buffer,
        buffer_size,
        "\"%s\" is not a function",
        compiler->last_error_extra_info);
    break;
  case ERROR_INVALID_FIELD_NAME:
    snprintf(
        buffer,
        buffer_size,
        "invalid field name \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_MISPLACED_ANONYMOUS_STRING:
    snprintf(buffer, buffer_size, "wrong use of anonymous string");
    break;
  case ERROR_INCLUDES_CIRCULAR_REFERENCE:
    snprintf(buffer, buffer_size, "include circular reference");
    break;
  case ERROR_INCLUDE_DEPTH_EXCEEDED:
    snprintf(buffer, buffer_size, "too many levels of included rules");
    break;
  case ERROR_LOOP_NESTING_LIMIT_EXCEEDED:
    snprintf(buffer, buffer_size, "loop nesting limit exceeded");
    break;
  case ERROR_NESTED_FOR_OF_LOOP:
    snprintf(
        buffer,
        buffer_size,
        "'for <quantifier> of <string set>' loops can't be nested");
    break;
  case ERROR_UNKNOWN_MODULE:
    snprintf(
        buffer,
        buffer_size,
        "unknown module \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_INVALID_MODULE_NAME:
    snprintf(
        buffer,
        buffer_size,
        "invalid module name \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_DUPLICATED_STRUCTURE_MEMBER:
    snprintf(buffer, buffer_size, "duplicated structure member");
    break;
  case ERROR_WRONG_ARGUMENTS:
    snprintf(
        buffer,
        buffer_size,
        "wrong arguments for function \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_WRONG_RETURN_TYPE:
    snprintf(buffer, buffer_size, "wrong return type for overloaded function");
    break;
  case ERROR_INVALID_HEX_STRING:
  case ERROR_INVALID_REGULAR_EXPRESSION:
  case ERROR_SYNTAX_ERROR:
  case ERROR_WRONG_TYPE:
  case ERROR_INVALID_MODIFIER:
  case ERROR_INVALID_PERCENTAGE:
    snprintf(buffer, buffer_size, "%s", compiler->last_error_extra_info);
    break;
  case ERROR_INTERNAL_FATAL_ERROR:
    snprintf(buffer, buffer_size, "internal fatal error");
    break;
  case ERROR_DIVISION_BY_ZERO:
    snprintf(buffer, buffer_size, "division by zero");
    break;
  case ERROR_REGULAR_EXPRESSION_TOO_LARGE:
    snprintf(buffer, buffer_size, "regular expression is too large");
    break;
  case ERROR_REGULAR_EXPRESSION_TOO_COMPLEX:
    snprintf(buffer, buffer_size, "regular expression is too complex");
    break;
  case ERROR_TOO_MANY_STRINGS:
    yr_get_configuration(YR_CONFIG_MAX_STRINGS_PER_RULE, &max_strings_per_rule);
    snprintf(
        buffer,
        buffer_size,
        "too many strings in rule \"%s\" (limit: %d)",
        compiler->last_error_extra_info,
        max_strings_per_rule);
    break;
  case ERROR_INTEGER_OVERFLOW:
    snprintf(
        buffer,
        buffer_size,
        "integer overflow in \"%s\"",
        compiler->last_error_extra_info);
    break;
  case ERROR_COULD_NOT_READ_FILE:
    snprintf(buffer, buffer_size, "could not read file");
    break;
  case ERROR_DUPLICATED_MODIFIER:
    snprintf(buffer, buffer_size, "duplicated modifier");
    break;
  }

  return buffer;
}