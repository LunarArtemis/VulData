static pointer mk_proc(scheme *sc, enum scheme_opcodes op) {
     pointer y;

     y = get_cell(sc, sc->NIL, sc->NIL);
     typeflag(y) = (T_PROC | T_ATOM);
     ivalue_unchecked(y) = (long) op;
     set_num_integer(y);
     return y;
}