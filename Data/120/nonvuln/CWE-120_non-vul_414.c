static pointer get_vector_object(scheme *sc, int len, pointer init)
{
  pointer cells = get_consecutive_cells(sc,len/2+len%2+1);
  if(sc->no_memory) { return sc->sink; }
  /* Record it as a vector so that gc understands it. */
  typeflag(cells) = (T_VECTOR | T_ATOM);
  ivalue_unchecked(cells)=len;
  set_num_integer(cells);
  fill_vector(cells,init);
  push_recent_alloc(sc, cells, sc->NIL);
  return cells;
}