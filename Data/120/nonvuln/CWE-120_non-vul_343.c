INTERFACE static pointer vector_elem(pointer vec, int ielem) {
     int n=ielem/2;
     if(ielem%2==0) {
          return car(vec+1+n);
     } else {
          return cdr(vec+1+n);
     }
}