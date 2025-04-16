static char getsymbol(unsigned char code, bool urlsafe)
{
    if (!IS_ACTIVE(MODULE_BASE64URL)) {
        urlsafe = false;
    }

    if (urlsafe && code == BASE64_UNDERLINE) {
        return '_';
    }

    if (urlsafe && code == BASE64_MINUS) {
        return '-';
    }

    if (!urlsafe && code == BASE64_SLASH) {
        return '/';
    }

    if (!urlsafe && code == BASE64_PLUS) {
        return '+';
    }

    if (code <= BASE64_CAPITAL_UPPER_BOUND) {
        return (code + 'A');
    }

    if (code <= BASE64_SMALL_UPPER_BOUND) {
        return (code + ('z' - BASE64_SMALL_UPPER_BOUND));
    }

    if (code <= BASE64_NUMBER_UPPER_BOUND) {
        return (code + ('9' - BASE64_NUMBER_UPPER_BOUND));
    }

    return (char)BASE64_NOT_DEFINED;
}