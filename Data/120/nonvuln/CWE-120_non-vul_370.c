INTERFACE pointer mk_counted_string(scheme *sc, const char *str, int len) {
     pointer x = get_cell(sc, sc->NIL, sc->NIL);

     typeflag(x) = (T_STRING | T_ATOM);
     strvalue(x) = store_string(sc,len,str,0);
     strlength(x) = len;
     return (x);
}