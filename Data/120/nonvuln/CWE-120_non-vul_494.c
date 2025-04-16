INTERFACE pointer mk_real(scheme *sc, double n) {
  pointer x = get_cell(sc,sc->NIL, sc->NIL);

  typeflag(x) = (T_NUMBER | T_ATOM);
  rvalue_unchecked(x)= n;
  set_num_real(x);
  return (x);
}