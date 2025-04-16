static pointer mk_number(scheme *sc, num n) {
 if(n.is_fixnum) {
     return mk_integer(sc,n.value.ivalue);
 } else {
     return mk_real(sc,n.value.rvalue);
 }
}