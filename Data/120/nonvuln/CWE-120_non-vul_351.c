int is_real(pointer p) {
  return is_number(p) && (!(p)->_object._number.is_fixnum);
}