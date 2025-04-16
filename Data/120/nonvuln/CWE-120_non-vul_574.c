static inline const char *AnsiSetFG() {
    return colorbits == 8 ? PIXEL_SET_FG_COLOR8 : PIXEL_SET_FG_COLOR24;
}