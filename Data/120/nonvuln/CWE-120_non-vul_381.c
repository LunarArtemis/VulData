static INLINE pointer get_cell_x(scheme *sc, pointer a, pointer b) {
  if (sc->free_cell != sc->NIL) {
    pointer x = sc->free_cell;
    sc->free_cell = cdr(x);
    --sc->fcells;
    return (x);
  }
  return _get_cell (sc, a, b);
}