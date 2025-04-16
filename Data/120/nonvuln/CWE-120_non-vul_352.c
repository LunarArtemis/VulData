INTERFACE static void fill_vector(pointer vec, pointer obj) {
     int i;
     int num=ivalue(vec)/2+ivalue(vec)%2;
     for(i=0; i<num; i++) {
          typeflag(vec+1+i) = T_PAIR;
          setimmutable(vec+1+i);
          car(vec+1+i)=obj;
          cdr(vec+1+i)=obj;
     }
}