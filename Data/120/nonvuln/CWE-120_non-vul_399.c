void scheme_define(scheme *sc, pointer envir, pointer symbol, pointer value) {
     pointer x;

     x=find_slot_in_env(sc,envir,symbol,0);
     if (x != sc->NIL) {
          set_slot_in_env(sc, x, value);
     } else {
          new_slot_spec_in_env(sc, envir, symbol, value);
     }
}