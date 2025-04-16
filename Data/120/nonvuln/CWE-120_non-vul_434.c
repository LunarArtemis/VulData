static pointer get_consecutive_cells(scheme *sc, int n) {
  pointer x;

  if (sc->no_memory) { return sc->sink; }

  /* Are there any cells available? */
  x=find_consecutive_cells(sc,n);
  if (x != sc->NIL) { return x; }

  /* If not, try gc'ing some */
  gc(sc, sc->NIL, sc->NIL);
  x=find_consecutive_cells(sc,n);
  if (x != sc->NIL) { return x; }

  /* If there still aren't, try getting more heap */
  if (!alloc_cellseg(sc,1))
    {
      sc->no_memory=1;
      return sc->sink;
    }

  x=find_consecutive_cells(sc,n);
  if (x != sc->NIL) { return x; }

  /* If all fail, report failure */
  sc->no_memory=1;
  return sc->sink;
}