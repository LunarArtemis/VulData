INTERFACE static pointer set_vector_elem(pointer vec, int ielem, pointer a) {
     int n=ielem/2;
     if(ielem%2==0) {
          return car(vec+1+n)=a;
     } else {
          return cdr(vec+1+n)=a;
     }
}