static INLINE void dump_stack_initialize(scheme *sc)
{
  sc->dump_size = 0;
  sc->dump_base = NULL;
  dump_stack_reset(sc);
}