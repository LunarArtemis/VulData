static void gc(scheme *sc, pointer a, pointer b) {
  pointer p;
  int i;

  if(sc->gc_verbose) {
    putstr(sc, "gc...");
  }

  /* mark system globals */
  mark(sc->oblist);
  mark(sc->global_env);

  /* mark current registers */
  mark(sc->args);
  mark(sc->envir);
  mark(sc->code);
  dump_stack_mark(sc);
  mark(sc->value);
  mark(sc->inport);
  mark(sc->save_inport);
  mark(sc->outport);
  mark(sc->loadport);

  /* Mark recent objects the interpreter doesn't know about yet. */
  mark(car(sc->sink));

  /* mark variables a, b */
  mark(a);
  mark(b);

  /* garbage collect */
  clrmark(sc->NIL);
  sc->fcells = 0;
  sc->free_cell = sc->NIL;
  /* free-list is kept sorted by address so as to maintain consecutive
     ranges, if possible, for use with vectors. Here we scan the cells
     (which are also kept sorted by address) downwards to build the
     free-list in sorted order.
  */
  for (i = sc->last_cell_seg; i >= 0; i--) {
    p = sc->cell_seg[i] + CELL_SEGSIZE;
    while (--p >= sc->cell_seg[i]) {
      if (is_mark(p)) {
        clrmark(p);
      } else {
        /* reclaim cell */
        if (typeflag(p) != 0) {
          finalize_cell(sc, p);
          typeflag(p) = 0;
          car(p) = sc->NIL;
        }
        ++sc->fcells;
        cdr(p) = sc->free_cell;
        sc->free_cell = p;
      }
    }
  }

  if (sc->gc_verbose) {
    char msg[80];
    sprintf(msg,"done: %ld cells were recovered.\n", sc->fcells);
    putstr(sc,msg);
  }
}