INTERFACE pointer mk_integer(scheme *sc, long num) {
  pointer x = get_cell(sc,sc->NIL, sc->NIL);

  typeflag(x) = (T_NUMBER | T_ATOM);
  ivalue_unchecked(x)= num;
  set_num_integer(x);
  return (x);
}