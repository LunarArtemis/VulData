static pointer get_cell(scheme *sc, pointer a, pointer b)
{
  pointer cell   = get_cell_x(sc, a, b);
  /* For right now, include "a" and "b" in "cell" so that gc doesn't
     think they are garbage. */
  /* Tentatively record it as a pair so gc understands it. */
  typeflag(cell) = T_PAIR;
  car(cell) = a;
  cdr(cell) = b;
  push_recent_alloc(sc, cell, sc->NIL);
  return cell;
}