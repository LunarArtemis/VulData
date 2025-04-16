static INLINE void new_slot_in_env(scheme *sc, pointer variable, pointer value)
{
  new_slot_spec_in_env(sc, sc->envir, variable, value);
}