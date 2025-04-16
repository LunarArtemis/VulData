int scheme_init(scheme *sc) {
 return scheme_init_custom_alloc(sc,malloc,free);
}