static pointer mk_continuation(scheme *sc, pointer d) {
     pointer x = get_cell(sc, sc->NIL, d);

     typeflag(x) = T_CONTINUATION;
     cont_dump(x) = d;
     return (x);
}