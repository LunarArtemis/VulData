pointer mk_foreign_func(scheme *sc, foreign_func f) {
  pointer x = get_cell(sc, sc->NIL, sc->NIL);

  typeflag(x) = (T_FOREIGN | T_ATOM);
  x->_object._ff=f;
  return (x);
}