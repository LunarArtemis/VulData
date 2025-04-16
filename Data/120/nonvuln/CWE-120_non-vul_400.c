static pointer _Error_1(scheme *sc, const char *s, pointer a) {
#if USE_ERROR_HOOK
     pointer x;
     pointer hdl=sc->ERROR_HOOK;

     x=find_slot_in_env(sc,sc->envir,hdl,1);
    if (x != sc->NIL) {
         if(a!=0) {
               sc->code = cons(sc, cons(sc, sc->QUOTE, cons(sc,(a), sc->NIL)), sc->NIL);
         } else {
               sc->code = sc->NIL;
         }
         sc->code = cons(sc, mk_string(sc, (s)), sc->code);
         setimmutable(car(sc->code));
         sc->code = cons(sc, slot_value_in_env(x), sc->code);
         sc->op = (int)OP_EVAL;
         return sc->T;
    }
#endif

    if(a!=0) {
          sc->args = cons(sc, (a), sc->NIL);
    } else {
          sc->args = sc->NIL;
    }
    sc->args = cons(sc, mk_string(sc, (s)), sc->args);
    setimmutable(car(sc->args));
    sc->op = (int)OP_ERR0;
    return sc->T;
}