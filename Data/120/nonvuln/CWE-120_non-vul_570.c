static char *AnsiWriteColor(char *buf, rgba_t color) {
    static_assert(colorbits == 8 || colorbits == 24, "unsupported color bits");
    if (colorbits == 8)
        return int_append_with_semicolon(buf, color.As256TermColor());

    buf = int_append_with_semicolon(buf, color.r);
    buf = int_append_with_semicolon(buf, color.g);
    return int_append_with_semicolon(buf, color.b);
}