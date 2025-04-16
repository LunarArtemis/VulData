static int is_nonneg(pointer p) {
  return is_integer(p) && ivalue(p)>=0;
}