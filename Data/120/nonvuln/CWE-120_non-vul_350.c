pointer foreign_error (scheme *sc, const char *s, pointer a) {
  sc->foreign_error = cons (sc, mk_string (sc, s), a);
  return sc->T;
}