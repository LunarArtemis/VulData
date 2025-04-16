static pointer _s_return(scheme *sc, pointer a)
{
  int nframes = (int)sc->dump;
  struct dump_stack_frame *frame;

  sc->value = (a);
  if (nframes <= 0) {
    return sc->NIL;
  }
  nframes--;
  frame = (struct dump_stack_frame *)sc->dump_base + nframes;
  sc->op = frame->op;
  sc->args = frame->args;
  sc->envir = frame->envir;
  sc->code = frame->code;
  sc->dump = (pointer)nframes;
  return sc->T;
}