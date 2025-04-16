static INLINE void dump_stack_reset(scheme *sc)
{
  /* in this implementation, sc->dump is the number of frames on the stack */
  sc->dump = (pointer)0;
}