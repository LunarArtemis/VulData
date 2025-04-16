INTERFACE pointer mk_character(scheme *sc, gunichar c) {
  pointer x = get_cell(sc,sc->NIL, sc->NIL);

  typeflag(x) = (T_CHARACTER | T_ATOM);
  ivalue_unchecked(x)= c;
  set_num_integer(x);
  return (x);
}