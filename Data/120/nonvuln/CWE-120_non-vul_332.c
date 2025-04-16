static void s_save(scheme *sc, enum scheme_opcodes op, pointer args, pointer code)
{
  int nframes = (int)sc->dump;
  struct dump_stack_frame *next_frame;

  /* enough room for the next frame? */
  if (nframes >= sc->dump_size) {
    sc->dump_size += STACK_GROWTH;
    /* alas there is no sc->realloc */
    sc->dump_base = realloc(sc->dump_base,
                            sizeof(struct dump_stack_frame) * sc->dump_size);
  }
  next_frame = (struct dump_stack_frame *)sc->dump_base + nframes;
  next_frame->op = op;
  next_frame->args = args;
  next_frame->envir = sc->envir;
  next_frame->code = code;
  sc->dump = (pointer)(nframes+1);
}