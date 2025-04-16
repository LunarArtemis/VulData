static INLINE void ok_to_freely_gc(scheme *sc)
{
  car(sc->sink) = sc->NIL;
}