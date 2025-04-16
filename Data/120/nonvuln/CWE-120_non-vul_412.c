pointer _cons(scheme *sc, pointer a, pointer b, int immutable) {
  pointer x = get_cell(sc,a, b);

  typeflag(x) = T_PAIR;
  if(immutable) {
    setimmutable(x);
  }
  car(x) = a;
  cdr(x) = b;
  return (x);
}