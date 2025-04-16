static INLINE pointer oblist_find_by_name(scheme *sc, const char *name)
{
  int location;
  pointer x;
  char *s;

  location = hash_fn(name, ivalue_unchecked(sc->oblist));
  for (x = vector_elem(sc->oblist, location); x != sc->NIL; x = cdr(x)) {
    s = symname(car(x));
    /* case-insensitive, per R5RS section 2. */
    if(stricmp(name, s) == 0) {
      return car(x);
    }
  }
  return sc->NIL;
}