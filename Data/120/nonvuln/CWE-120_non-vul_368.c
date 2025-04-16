int is_integer(pointer p) {
  return is_number(p) && ((p)->_object._number.is_fixnum);
}