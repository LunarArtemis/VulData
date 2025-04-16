static INLINE void set_slot_in_env(scheme *sc, pointer slot, pointer value)
{
  cdr(slot) = value;
}