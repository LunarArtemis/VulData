static pointer _get_cell(scheme *sc, pointer a, pointer b) {
  pointer x;

  if(sc->no_memory) {
    return sc->sink;
  }

  if (sc->free_cell == sc->NIL) {
    const int min_to_be_recovered = sc->last_cell_seg*8;
    gc(sc,a, b);
    if (sc->fcells < min_to_be_recovered
        || sc->free_cell == sc->NIL) {
      /* if only a few recovered, get more to avoid fruitless gc's */
      if (!alloc_cellseg(sc,1) && sc->free_cell == sc->NIL) {
        sc->no_memory=1;
        return sc->sink;
      }
    }
  }
  x = sc->free_cell;
  sc->free_cell = cdr(x);
  --sc->fcells;
  return (x);
}