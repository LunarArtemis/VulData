void scheme_load_string(scheme *sc, const char *cmd) {
  dump_stack_reset(sc);
  sc->envir = sc->global_env;
  sc->file_i=0;
  sc->load_stack[0].kind=port_input|port_string;
  sc->load_stack[0].rep.string.start=(char*)cmd; /* This func respects const */
  sc->load_stack[0].rep.string.past_the_end=(char*)cmd+strlen(cmd);
  sc->load_stack[0].rep.string.curr=(char*)cmd;
  sc->loadport=mk_port(sc,sc->load_stack);
  sc->retcode=0;
  sc->interactive_repl=0;
  sc->inport=sc->loadport;
  Eval_Cycle(sc, OP_T0LVL);
  typeflag(sc->loadport)=T_ATOM;
  if(sc->retcode==0) {
    sc->retcode=sc->nesting!=0;
  }
}