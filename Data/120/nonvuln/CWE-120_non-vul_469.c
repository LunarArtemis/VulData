static INLINE int num_is_integer(pointer p) {
  return ((p)->_object._number.is_fixnum);
}