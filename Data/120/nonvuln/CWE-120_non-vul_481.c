static pointer oblist_all_symbols(scheme *sc)
{
  int i;
  pointer x;
  pointer ob_list = sc->NIL;

  for (i = 0; i < ivalue_unchecked(sc->oblist); i++) {
    for (x  = vector_elem(sc->oblist, i); x != sc->NIL; x = cdr(x)) {
      ob_list = cons(sc, x, ob_list);
    }
  }
  return ob_list;
}