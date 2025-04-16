pointer scheme_apply0(scheme *sc, const char *procname)
{ return scheme_eval(sc, cons(sc,mk_symbol(sc,procname),sc->NIL)); }