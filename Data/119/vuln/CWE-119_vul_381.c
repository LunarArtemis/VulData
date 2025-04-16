static int getcode(char symbol)
{
    if (symbol == '/') {
        return BASE64_SLASH;
    }

    if (symbol == '_') {
        return BASE64_UNDERLINE;
    }

    if (symbol == '+') {
        return BASE64_PLUS;
    }

    if (symbol == '-') {
        return BASE64_MINUS;
    }

    if (symbol == '=') {
        /* indicates a padded base64 end */
        return BASE64_EQUALS;
    }

    if (symbol < '0') {
        /* indicates that the given symbol is not base64 and should be ignored */
        return BASE64_NOT_DEFINED;
    }

    if (symbol <= '9' && symbol >= '0') {
        return (symbol + (BASE64_NUMBER_UPPER_BOUND - '9'));
    }

    if (symbol <= 'Z' && symbol >= 'A') {
        return (symbol - 'A');
    }

    if (symbol <= 'z' && symbol >= 'a') {
        return (symbol + (BASE64_SMALL_UPPER_BOUND - 'z'));
    }

    /* indicates that the given symbol is not base64 and should be ignored */
    return BASE64_NOT_DEFINED;
}