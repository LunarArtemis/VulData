INTERFACE pointer mk_symbol(scheme *sc, const char *name) {
     pointer x;

     /* first check oblist */
     x = oblist_find_by_name(sc, name);
     if (x != sc->NIL) {
          return (x);
     } else {
          x = oblist_add_by_name(sc, name);
          return (x);
     }
}