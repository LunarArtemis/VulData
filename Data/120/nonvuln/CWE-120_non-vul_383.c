pointer scheme_eval(scheme *sc, pointer obj)
{
  int old_repl = sc->interactive_repl;
  sc->interactive_repl = 0;
  s_save(sc,OP_QUIT,sc->NIL,sc->NIL);
  sc->args = sc->NIL;
  sc->code = obj;
  sc->retcode = 0;
  Eval_Cycle(sc, OP_EVAL);
  sc->interactive_repl = old_repl;
  return sc->value;
}