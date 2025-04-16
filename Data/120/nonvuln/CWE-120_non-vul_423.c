INTERFACE pointer mk_empty_string(scheme *sc, int len, gunichar fill) {
     pointer x = get_cell(sc, sc->NIL, sc->NIL);

     typeflag(x) = (T_STRING | T_ATOM);
     strvalue(x) = store_string(sc,len,0,fill);
     strlength(x) = len;
     return (x);
}