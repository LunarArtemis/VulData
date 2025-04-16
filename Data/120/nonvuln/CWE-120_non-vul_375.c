scheme *scheme_init_new_custom_alloc(func_alloc malloc, func_dealloc free) {
  scheme *sc=(scheme*)malloc(sizeof(scheme));
  if(!scheme_init_custom_alloc(sc,malloc,free)) {
    free(sc);
    return 0;
  } else {
    return sc;
  }
}