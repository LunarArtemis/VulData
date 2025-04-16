static pointer reserve_cells(scheme *sc, int n) {
       if(sc->no_memory) {
               return sc->NIL;
       }

       /* Are there enough cells available? */
       if (sc->fcells < n) {
               /* If not, try gc'ing some */
               gc(sc, sc->NIL, sc->NIL);
               if (sc->fcells < n) {
                       /* If there still aren't, try getting more heap */
                       if (!alloc_cellseg(sc,1)) {
                               sc->no_memory=1;
                               return sc->NIL;
                       }
               }
               if (sc->fcells < n) {
                       /* If all fail, report failure */
                       sc->no_memory=1;
                       return sc->NIL;
               }
       }
       return (sc->T);
}