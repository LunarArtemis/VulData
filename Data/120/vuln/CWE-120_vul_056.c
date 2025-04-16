static void atom2str(scheme *sc, pointer l, int f, char **pp, int *plen) {
     char *p;

     if (l == sc->NIL) {
          p = "()";
     } else if (l == sc->T) {
          p = "#t";
     } else if (l == sc->F) {
          p = "#f";
     } else if (l == sc->EOF_OBJ) {
          p = "#<EOF>";
     } else if (is_port(l)) {
          p = sc->strbuff;
          strcpy(p, "#<PORT>");
     } else if (is_number(l)) {
          p = sc->strbuff;
          if(num_is_integer(l)) {
               sprintf(p, "%ld", ivalue_unchecked(l));
          } else {
               g_ascii_formatd (p, sizeof (sc->strbuff), "%.10g",
                                rvalue_unchecked(l));
          }
     } else if (is_string(l)) {
          if (!f) {
               p = strvalue(l);
          } else { /* Hack, uses the fact that printing is needed */
               *pp=sc->strbuff;
               *plen=0;
               printslashstring(sc, strvalue(l),
                                g_utf8_strlen(strvalue(l), -1));
               return;
          }
     } else if (is_character(l)) {
          gunichar c=charvalue(l);
          p = sc->strbuff;
          if (!f) {
               int len = g_unichar_to_utf8(c, p);
               p[len]=0;
          } else {
               switch(c) {
               case ' ':
                    sprintf(p,"#\\space"); break;
               case '\n':
                    sprintf(p,"#\\newline"); break;
               case '\r':
                    sprintf(p,"#\\return"); break;
               case '\t':
                    sprintf(p,"#\\tab"); break;
               default:
#if USE_ASCII_NAMES
                    if(c==127) {
                         strcpy(p,"#\\del"); break;
                    } else if(c<32) {
                         strcpy(p,"#\\"); strcat(p,charnames[c]); break;
                    }
#else
                    if(c<32) {
                      sprintf(p,"#\\x%x",c); break;
                    }
#endif
                    sprintf(p,"#\\%c",c); break;
               }
          }
     } else if (is_symbol(l)) {
          p = symname(l);
     } else if (is_proc(l)) {
          p = sc->strbuff;
          sprintf(p, "#<%s PROCEDURE %ld>", procname(l),procnum(l));
     } else if (is_macro(l)) {
          p = "#<MACRO>";
     } else if (is_closure(l)) {
          p = "#<CLOSURE>";
     } else if (is_promise(l)) {
          p = "#<PROMISE>";
     } else if (is_foreign(l)) {
          p = sc->strbuff;
          sprintf(p, "#<FOREIGN PROCEDURE %ld>", procnum(l));
     } else if (is_continuation(l)) {
          p = "#<CONTINUATION>";
     } else {
          p = "#<ERROR>";
     }
     *pp=p;
     *plen=g_utf8_strlen(p, -1);
}