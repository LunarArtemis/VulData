INTERFACE void putstr(scheme *sc, const char *s) {
  putchars(sc, s, g_utf8_strlen(s, -1));
}