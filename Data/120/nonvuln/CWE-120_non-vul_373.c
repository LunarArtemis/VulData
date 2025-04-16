static int token(scheme *sc) {
     gunichar c;
     skipspace(sc);
     switch (c=inchar(sc)) {
     case EOF:
          return (TOK_EOF);
     case '(':
          return (TOK_LPAREN);
     case ')':
          return (TOK_RPAREN);
     case '.':
          c=inchar(sc);
          if(is_one_of(" \n\t",c)) {
               return (TOK_DOT);
          } else {
               backchar(sc,c);
               backchar(sc,'.');
               return TOK_ATOM;
          }
     case '\'':
          return (TOK_QUOTE);
     case ';':
          while ((c=inchar(sc)) != '\n' && c!=EOF)
            ;
          return (token(sc));
     case '"':
          return (TOK_DQUOTE);
     case '_':
          if ((c=inchar(sc)) == '"')
               return (TOK_DQUOTE);
          backchar(sc,c);
          return (TOK_ATOM);
     case BACKQUOTE:
          return (TOK_BQUOTE);
     case ',':
          if ((c=inchar(sc)) == '@')
               return (TOK_ATMARK);
          else {
               backchar(sc,c);
               return (TOK_COMMA);
          }
     case '#':
          c=inchar(sc);
          if (c == '(') {
               return (TOK_VEC);
          } else if(c == '!') {
               while ((c=inchar(sc)) != '\n' && c!=EOF)
                   ;
               return (token(sc));
          } else {
               backchar(sc,c);
               if(is_one_of(" tfodxb\\",c)) {
                    return TOK_SHARP_CONST;
               } else {
                    return (TOK_SHARP);
               }
          }
     default:
          backchar(sc,c);
          return (TOK_ATOM);
     }
}