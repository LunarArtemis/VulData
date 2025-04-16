INTERFACE pointer gensym(scheme *sc) {
     pointer x;
     char name[40];

     for(; sc->gensym_cnt<LONG_MAX; sc->gensym_cnt++) {
          sprintf(name,"gensym-%ld",sc->gensym_cnt);

          /* first check oblist */
          x = oblist_find_by_name(sc, name);

          if (x != sc->NIL) {
               continue;
          } else {
               x = oblist_add_by_name(sc, name);
               return (x);
          }
     }

     return sc->NIL;
}