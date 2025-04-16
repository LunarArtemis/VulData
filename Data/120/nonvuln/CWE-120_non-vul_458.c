void scheme_deinit(scheme *sc) {
  int i;

  sc->oblist=sc->NIL;
  sc->global_env=sc->NIL;
  dump_stack_free(sc);
  sc->envir=sc->NIL;
  sc->code=sc->NIL;
  sc->args=sc->NIL;
  sc->value=sc->NIL;
  if(is_port(sc->inport)) {
    typeflag(sc->inport) = T_ATOM;
  }
  sc->inport=sc->NIL;
  sc->outport=sc->NIL;
  if(is_port(sc->save_inport)) {
    typeflag(sc->save_inport) = T_ATOM;
  }
  sc->save_inport=sc->NIL;
  if(is_port(sc->loadport)) {
    typeflag(sc->loadport) = T_ATOM;
  }
  sc->loadport=sc->NIL;
  sc->gc_verbose=0;
  gc(sc,sc->NIL,sc->NIL);

  for(i=0; i<=sc->last_cell_seg; i++) {
    sc->free(sc->alloc_seg[i]);
  }
}