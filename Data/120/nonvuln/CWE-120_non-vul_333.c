pointer scheme_call(scheme *sc, pointer func, pointer args)
{
  int old_repl = sc->interactive_repl;
  sc->interactive_repl = 0;
  s_save(sc,OP_QUIT,sc->NIL,sc->NIL);
  sc->envir = sc->global_env;
  sc->args = args;
  sc->code = func;
  sc->retcode = 0;
  Eval_Cycle(sc, OP_APPLY);
  sc->interactive_repl = old_repl;
  return sc->value;
}