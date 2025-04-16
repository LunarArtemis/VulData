static pointer oblist_add_by_name(scheme *sc, const char *name)
{
  pointer x;
  int location;

  x = immutable_cons(sc, mk_string(sc, name), sc->NIL);
  typeflag(x) = T_SYMBOL;
  setimmutable(car(x));

  location = hash_fn(name, ivalue_unchecked(sc->oblist));
  set_vector_elem(sc->oblist, location,
                  immutable_cons(sc, x, vector_elem(sc->oblist, location)));
  return x;
}