static int alloc_cellseg(scheme *sc, int n) {
     pointer newp;
     pointer last;
     pointer p;
     char *cp;
     long i;
     int k;
     int adj=ADJ;

     if(adj<sizeof(struct cell)) {
       adj=sizeof(struct cell);
     }

     for (k = 0; k < n; k++) {
          if (sc->last_cell_seg >= CELL_NSEGMENT - 1)
               return k;
          cp = (char*) sc->malloc(CELL_SEGSIZE * sizeof(struct cell)+adj);
          if (cp == 0)
               return k;
          i = ++sc->last_cell_seg ;
          sc->alloc_seg[i] = cp;
          /* adjust in TYPE_BITS-bit boundary */
          if (((unsigned long) cp) % adj != 0) {
            cp=(char*)(adj*((unsigned long)cp/adj+1));
          }
        /* insert new segment in address order */
          newp=(pointer)cp;
        sc->cell_seg[i] = newp;
        while (i > 0 && sc->cell_seg[i - 1] > sc->cell_seg[i]) {
              p = sc->cell_seg[i];
            sc->cell_seg[i] = sc->cell_seg[i - 1];
            sc->cell_seg[--i] = p;
        }
          sc->fcells += CELL_SEGSIZE;
        last = newp + CELL_SEGSIZE - 1;
          for (p = newp; p <= last; p++) {
               typeflag(p) = 0;
               cdr(p) = p + 1;
               car(p) = sc->NIL;
          }
        /* insert new cells in address order on free list */
        if (sc->free_cell == sc->NIL || p < sc->free_cell) {
             cdr(last) = sc->free_cell;
             sc->free_cell = newp;
        } else {
              p = sc->free_cell;
              while (cdr(p) != sc->NIL && newp > cdr(p))
                   p = cdr(p);
              cdr(last) = cdr(p);
              cdr(p) = newp;
        }
     }
     return n;
}