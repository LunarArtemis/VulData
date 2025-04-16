static pointer find_slot_in_env(scheme *sc, pointer env, pointer hdl, int all)
{
  pointer x,y;
  int location;

  for (x = env; x != sc->NIL; x = cdr(x)) {
    if (is_vector(car(x))) {
      location = hash_fn(symname(hdl), ivalue_unchecked(car(x)));
      y = vector_elem(car(x), location);
    } else {
      y = car(x);
    }
    for ( ; y != sc->NIL; y = cdr(y)) {
              if (caar(y) == hdl) {
                   break;
              }
         }
         if (y != sc->NIL) {
              break;
         }
         if(!all) {
           return sc->NIL;
         }
    }
    if (x != sc->NIL) {
          return car(y);
    }
    return sc->NIL;
}