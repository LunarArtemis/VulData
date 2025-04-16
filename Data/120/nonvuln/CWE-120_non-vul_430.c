INTERFACE pointer mk_string(scheme *sc, const char *str) {
     return mk_counted_string(sc,str,g_utf8_strlen(str, -1));
}