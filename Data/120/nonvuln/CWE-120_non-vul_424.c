static pointer mk_sharp_const(scheme *sc, char *name) {
     long    x;
     char    tmp[256];

     if (!strcmp(name, "t"))
          return (sc->T);
     else if (!strcmp(name, "f"))
          return (sc->F);
     else if (*name == 'o') {/* #o (octal) */
          sprintf(tmp, "0%s", name+1);
          sscanf(tmp, "%lo", &x);
          return (mk_integer(sc, x));
     } else if (*name == 'd') {    /* #d (decimal) */
          sscanf(name+1, "%ld", &x);
          return (mk_integer(sc, x));
     } else if (*name == 'x') {    /* #x (hex) */
          sprintf(tmp, "0x%s", name+1);
          sscanf(tmp, "%lx", &x);
          return (mk_integer(sc, x));
     } else if (*name == 'b') {    /* #b (binary) */
          x = binary_decode(name+1);
          return (mk_integer(sc, x));
     } else if (*name == '\\') { /* #\w (character) */
          gunichar c=0;
          if(stricmp(name+1,"space")==0) {
               c=' ';
          } else if(stricmp(name+1,"newline")==0) {
               c='\n';
          } else if(stricmp(name+1,"return")==0) {
               c='\r';
          } else if(stricmp(name+1,"tab")==0) {
               c='\t';
     } else if(name[1]=='x' && name[2]!=0) {
          int c1=0;
          if(sscanf(name+2,"%x",&c1)==1 && c1<256) {
               c=c1;
          } else {
               return sc->NIL;
     }
#if USE_ASCII_NAMES
          } else if(is_ascii_name(name+1,&c)) {
               /* nothing */
#endif
          } else if(name[2]==0) {
               c=name[1];
          } else {
               return sc->NIL;
          }
          return mk_character(sc,c);
     } else
          return (sc->NIL);
}