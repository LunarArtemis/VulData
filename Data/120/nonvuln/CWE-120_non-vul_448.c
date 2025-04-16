static pointer mk_closure(scheme *sc, pointer c, pointer e) {
     pointer x = get_cell(sc, c, e);

     typeflag(x) = T_CLOSURE;
     car(x) = c;
     cdr(x) = e;
     return (x);
}