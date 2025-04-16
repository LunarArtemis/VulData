static void mark(pointer a) {
     pointer t, q, p;

     t = (pointer) 0;
     p = a;
E2:  setmark(p);
     if(is_vector(p)) {
          int i;
          int num=ivalue_unchecked(p)/2+ivalue_unchecked(p)%2;
          for(i=0; i<num; i++) {
               /* Vector cells will be treated like ordinary cells */
               mark(p+1+i);
          }
     }
     if (is_atom(p))
          goto E6;
     /* E4: down car */
     q = car(p);
     if (q && !is_mark(q)) {
          setatom(p);  /* a note that we have moved car */
          car(p) = t;
          t = p;
          p = q;
          goto E2;
     }
 E5:  q = cdr(p); /* down cdr */
     if (q && !is_mark(q)) {
          cdr(p) = t;
          t = p;
          p = q;
          goto E2;
     }
E6:   /* up.  Undo the link switching from steps E4 and E5. */
     if (!t)
          return;
     q = t;
     if (is_atom(q)) {
          clratom(q);
          t = car(q);
          car(q) = p;
          p = q;
          goto E5;
     } else {
          t = cdr(q);
          cdr(q) = p;
          p = q;
          goto E6;
     }
}