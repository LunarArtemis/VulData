static pointer append(scheme *sc, pointer a, pointer b) {
     pointer p = b, q;

     if (a != sc->NIL) {
          a = reverse(sc, a);
          while (a != sc->NIL) {
               q = cdr(a);
               cdr(a) = p;
               p = a;
               a = q;
          }
     }
     return (p);
}