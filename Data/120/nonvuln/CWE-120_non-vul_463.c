static pointer list_star(scheme *sc, pointer d) {
  pointer p, q;
  if(cdr(d)==sc->NIL) {
    return car(d);
  }
  p=cons(sc,car(d),cdr(d));
  q=p;
  while(cdr(cdr(p))!=sc->NIL) {
    d=cons(sc,car(p),cdr(p));
    if(cdr(cdr(p))!=sc->NIL) {
      p=cdr(d);
    }
  }
  cdr(p)=car(cdr(p));
  return q;
}