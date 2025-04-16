static pointer reverse(scheme *sc, pointer a) {
/* a must be checked by gc */
     pointer p = sc->NIL;

     for ( ; is_pair(a); a = cdr(a)) {
          p = cons(sc, car(a), p);
     }
     return (p);
}