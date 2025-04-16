scheme *scheme_init_new(void) {
  scheme *sc=(scheme*)malloc(sizeof(scheme));
  if(!scheme_init(sc)) {
    free(sc);
    return 0;
  } else {
    return sc;
  }
}