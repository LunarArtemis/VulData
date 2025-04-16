void scheme_load_file(scheme *sc, FILE *fin) {
  dump_stack_reset(sc);
  sc->envir = sc->global_env;
  sc->file_i=0;
  sc->load_stack[0].kind=port_input|port_file;
  sc->load_stack[0].rep.stdio.file=fin;
  sc->loadport=mk_port(sc,sc->load_stack);
  sc->retcode=0;
  if(fin==stdin) {
    sc->interactive_repl=1;
  }
  sc->inport=sc->loadport;
  Eval_Cycle(sc, OP_T0LVL);
  typeflag(sc->loadport)=T_ATOM;
  if(sc->retcode==0) {
    sc->retcode=sc->nesting!=0;
  }
}