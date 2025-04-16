static num num_mod(num a, num b) {
 num ret;
 long e1, e2, res;
 ret.is_fixnum=a.is_fixnum && b.is_fixnum;
 e1=num_ivalue(a);
 e2=num_ivalue(b);
 res=e1%e2;
 if(res*e2<0) {    /* modulo should have same sign as second operand */
     e2=labs(e2);
     if(res>0) {
          res-=e2;
     } else {
          res+=e2;
     }
 }
 ret.value.ivalue=res;
 return ret;
}