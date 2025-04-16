static void dump_stack_free(scheme *sc)
{
  free(sc->dump_base);
  sc->dump_base = NULL;
  sc->dump = (pointer)0;
  sc->dump_size = 0;
}