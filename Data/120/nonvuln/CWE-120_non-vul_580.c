static inline const char *AnsiSetBG() {
    return colorbits == 8 ? PIXEL_SET_BG_COLOR8 : PIXEL_SET_BG_COLOR24;
}