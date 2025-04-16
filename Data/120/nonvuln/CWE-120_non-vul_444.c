static pointer mk_atom(scheme *sc, char *q) {
     char    c, *p;
     int has_dec_point=0;
     int has_fp_exp = 0;

#if USE_COLON_HOOK
     if((p=strstr(q,"::"))!=0) {
          *p=0;
          return cons(sc, sc->COLON_HOOK,
                          cons(sc,
                              cons(sc,
                                   sc->QUOTE,
                                   cons(sc, mk_atom(sc,p+2), sc->NIL)),
                              cons(sc, mk_symbol(sc,strlwr(q)), sc->NIL)));
     }
#endif

     p = q;
     c = *p++;
     if ((c == '+') || (c == '-')) {
       c = *p++;
       if (c == '.') {
         has_dec_point=1;
         c = *p++;
       }
       if (!isdigit(c)) {
         return (mk_symbol(sc, strlwr(q)));
       }
     } else if (c == '.') {
       has_dec_point=1;
       c = *p++;
       if (!isdigit(c)) {
         return (mk_symbol(sc, strlwr(q)));
       }
     } else if (!isdigit(c)) {
       return (mk_symbol(sc, strlwr(q)));
     }

     for ( ; (c = *p) != 0; ++p) {
          if (!isdigit(c)) {
               if(c=='.') {
                    if(!has_dec_point) {
                         has_dec_point=1;
                         continue;
                    }
               }
               else if ((c == 'e') || (c == 'E')) {
                       if(!has_fp_exp) {
                          has_dec_point = 1; /* decimal point illegal
                                                from now on */
                          p++;
                          if ((*p == '-') || (*p == '+') || isdigit(*p)) {
                             continue;
                          }
                       }
               }
               return (mk_symbol(sc, strlwr(q)));
          }
     }
     if(has_dec_point) {
       return mk_real(sc,g_ascii_strtod(q,NULL));
     }
     return (mk_integer(sc, atol(q)));
}