static pointer find_consecutive_cells(scheme *sc, int n) {
  pointer *pp;
  int cnt;

  pp=&sc->free_cell;
  while(*pp!=sc->NIL) {
    cnt=count_consecutive_cells(*pp,n);
    if(cnt>=n) {
      pointer x=*pp;
      *pp=cdr(*pp+n-1);
      sc->fcells -= n;
      return x;
    }
    pp=&cdr(*pp+cnt-1);
  }
  return sc->NIL;
}