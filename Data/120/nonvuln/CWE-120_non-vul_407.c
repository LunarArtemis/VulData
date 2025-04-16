static INLINE void dump_stack_mark(scheme *sc)
{
  int nframes = (int)sc->dump;
  int i;
  for(i=0; i<nframes; i++) {
    struct dump_stack_frame *frame;
    frame = (struct dump_stack_frame *)sc->dump_base + i;
    mark(frame->args);
    mark(frame->envir);
    mark(frame->code);
  }
}