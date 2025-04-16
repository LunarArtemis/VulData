static pointer opexe_5(scheme *sc, enum scheme_opcodes op) {
     pointer x;

     if(sc->nesting!=0) {
          int n=sc->nesting;
          sc->nesting=0;
          sc->retcode=-1;
          Error_1(sc,"unmatched parentheses:",mk_integer(sc,n));
     }

     switch (op) {
     /* ========== reading part ========== */
     case OP_READ:
          if(!is_pair(sc->args)) {
               s_goto(sc,OP_READ_INTERNAL);
          }
          if(!is_inport(car(sc->args))) {
               Error_1(sc,"read: not an input port:",car(sc->args));
          }
          if(car(sc->args)==sc->inport) {
               s_goto(sc,OP_READ_INTERNAL);
          }
          x=sc->inport;
          sc->inport=car(sc->args);
          x=cons(sc,x,sc->NIL);
          s_save(sc,OP_SET_INPORT, x, sc->NIL);
          s_goto(sc,OP_READ_INTERNAL);

     case OP_READ_CHAR: /* read-char */
     case OP_PEEK_CHAR: /* peek-char */ {
          gunichar c;
          if(is_pair(sc->args)) {
               if(car(sc->args)!=sc->inport) {
                    x=sc->inport;
                    x=cons(sc,x,sc->NIL);
                    s_save(sc,OP_SET_INPORT, x, sc->NIL);
                    sc->inport=car(sc->args);
               }
          }
          c=inchar(sc);
          if(c==EOF) {
               s_return(sc,sc->EOF_OBJ);
          }
          if(sc->op==OP_PEEK_CHAR) {
               backchar(sc,c);
          }
          s_return(sc,mk_character(sc,c));
     }

     case OP_CHAR_READY: /* char-ready? */ {
          pointer p=sc->inport;
          int res;
          if(is_pair(sc->args)) {
               p=car(sc->args);
          }
          res=p->_object._port->kind&port_string;
          s_retbool(res);
     }

     case OP_SET_INPORT: /* set-input-port */
          sc->inport=car(sc->args);
          s_return(sc,sc->value);

     case OP_SET_OUTPORT: /* set-output-port */
          sc->outport=car(sc->args);
          s_return(sc,sc->value);

     case OP_RDSEXPR:
          switch (sc->tok) {
          case TOK_EOF:
               if(sc->inport==sc->loadport) {
                    sc->args=sc->NIL;
                    s_goto(sc,OP_QUIT);
               } else {
                    s_return(sc,sc->EOF_OBJ);
               }
/*
 * Commented out because we now skip comments in the scanner
 *
          case TOK_COMMENT: {
               gunichar c;
               while ((c=inchar(sc)) != '\n' && c!=EOF)
                    ;
               sc->tok = token(sc);
               s_goto(sc,OP_RDSEXPR);
          }
*/
          case TOK_VEC:
               s_save(sc,OP_RDVEC,sc->NIL,sc->NIL);
               /* fall through */
          case TOK_LPAREN:
               sc->tok = token(sc);
               if (sc->tok == TOK_RPAREN) {
                    s_return(sc,sc->NIL);
               } else if (sc->tok == TOK_DOT) {
                    Error_0(sc,"syntax error: illegal dot expression");
               } else {
                    sc->nesting_stack[sc->file_i]++;
                    s_save(sc,OP_RDLIST, sc->NIL, sc->NIL);
                    s_goto(sc,OP_RDSEXPR);
               }
          case TOK_QUOTE:
               s_save(sc,OP_RDQUOTE, sc->NIL, sc->NIL);
               sc->tok = token(sc);
               s_goto(sc,OP_RDSEXPR);
          case TOK_BQUOTE:
               sc->tok = token(sc);
               if(sc->tok==TOK_VEC) {
                 s_save(sc,OP_RDQQUOTEVEC, sc->NIL, sc->NIL);
                 sc->tok=TOK_LPAREN;
                 s_goto(sc,OP_RDSEXPR);
               } else {
                 s_save(sc,OP_RDQQUOTE, sc->NIL, sc->NIL);
               }
               s_goto(sc,OP_RDSEXPR);
          case TOK_COMMA:
               s_save(sc,OP_RDUNQUOTE, sc->NIL, sc->NIL);
               sc->tok = token(sc);
               s_goto(sc,OP_RDSEXPR);
          case TOK_ATMARK:
               s_save(sc,OP_RDUQTSP, sc->NIL, sc->NIL);
               sc->tok = token(sc);
               s_goto(sc,OP_RDSEXPR);
          case TOK_ATOM:
               s_return(sc,mk_atom(sc, readstr_upto(sc, "();\t\n\r ")));
          case TOK_DQUOTE:
               x=readstrexp(sc);
               if(x==sc->F) {
                 Error_0(sc,"Error reading string");
               }
               setimmutable(x);
               s_return(sc,x);
          case TOK_SHARP: {
               pointer f=find_slot_in_env(sc,sc->envir,sc->SHARP_HOOK,1);
               if(f==sc->NIL) {
                    Error_0(sc,"undefined sharp expression");
               } else {
                    sc->code=cons(sc,slot_value_in_env(f),sc->NIL);
                    s_goto(sc,OP_EVAL);
               }
          }
          case TOK_SHARP_CONST:
               if ((x = mk_sharp_const(sc, readstr_upto(sc, "();\t\n\r "))) == sc->NIL) {
                    Error_0(sc,"undefined sharp expression");
               } else {
                    s_return(sc,x);
               }
          default:
               Error_1(sc, "syntax error: illegal token", mk_integer (sc, sc->tok));
          }
          break;

     case OP_RDLIST: {
          sc->args = cons(sc, sc->value, sc->args);
          sc->tok = token(sc);
/* We now skip comments in the scanner
          while (sc->tok == TOK_COMMENT) {
               gunichar c;
               while ((c=inchar(sc)) != '\n' && c!=EOF)
                    ;
               sc->tok = token(sc);
          }
*/
          if (sc->tok == TOK_RPAREN) {
               gunichar c = inchar(sc);
               if (c != '\n') backchar(sc,c);
               sc->nesting_stack[sc->file_i]--;
               s_return(sc,reverse_in_place(sc, sc->NIL, sc->args));
          } else if (sc->tok == TOK_DOT) {
               s_save(sc,OP_RDDOT, sc->args, sc->NIL);
               sc->tok = token(sc);
               s_goto(sc,OP_RDSEXPR);
          } else {
               s_save(sc,OP_RDLIST, sc->args, sc->NIL);
               s_goto(sc,OP_RDSEXPR);
          }
     }

     case OP_RDDOT:
          if (token(sc) != TOK_RPAREN) {
               Error_0(sc,"syntax error: illegal dot expression");
          } else {
               sc->nesting_stack[sc->file_i]--;
               s_return(sc,reverse_in_place(sc, sc->value, sc->args));
          }

     case OP_RDQUOTE:
          s_return(sc,cons(sc, sc->QUOTE, cons(sc, sc->value, sc->NIL)));

     case OP_RDQQUOTE:
          s_return(sc,cons(sc, sc->QQUOTE, cons(sc, sc->value, sc->NIL)));

     case OP_RDQQUOTEVEC:
       s_return(sc,cons(sc, mk_symbol(sc,"apply"),
                        cons(sc, mk_symbol(sc,"vector"),
                             cons(sc,cons(sc, sc->QQUOTE,
                                  cons(sc,sc->value,sc->NIL)),
                                  sc->NIL))));

     case OP_RDUNQUOTE:
          s_return(sc,cons(sc, sc->UNQUOTE, cons(sc, sc->value, sc->NIL)));

     case OP_RDUQTSP:
          s_return(sc,cons(sc, sc->UNQUOTESP, cons(sc, sc->value, sc->NIL)));

     case OP_RDVEC:
          /*sc->code=cons(sc,mk_proc(sc,OP_VECTOR),sc->value);
          s_goto(sc,OP_EVAL); Cannot be quoted*/
       /*x=cons(sc,mk_proc(sc,OP_VECTOR),sc->value);
         s_return(sc,x); Cannot be part of pairs*/
       /*sc->code=mk_proc(sc,OP_VECTOR);
       sc->args=sc->value;
       s_goto(sc,OP_APPLY);*/
       sc->args=sc->value;
       s_goto(sc,OP_VECTOR);

     /* ========== printing part ========== */
     case OP_P0LIST:
          if(is_vector(sc->args)) {
               putstr(sc,"#(");
               sc->args=cons(sc,sc->args,mk_integer(sc,0));
               s_goto(sc,OP_PVECFROM);
          } else if(is_environment(sc->args)) {
               putstr(sc,"#<ENVIRONMENT>");
               s_return(sc,sc->T);
          } else if (!is_pair(sc->args)) {
               printatom(sc, sc->args, sc->print_flag);
               s_return(sc,sc->T);
          } else if (car(sc->args) == sc->QUOTE && ok_abbrev(cdr(sc->args))) {
               putstr(sc, "'");
               sc->args = cadr(sc->args);
               s_goto(sc,OP_P0LIST);
          } else if (car(sc->args) == sc->QQUOTE && ok_abbrev(cdr(sc->args))) {
               putstr(sc, "`");
               sc->args = cadr(sc->args);
               s_goto(sc,OP_P0LIST);
          } else if (car(sc->args) == sc->UNQUOTE && ok_abbrev(cdr(sc->args))) {
               putstr(sc, ",");
               sc->args = cadr(sc->args);
               s_goto(sc,OP_P0LIST);
          } else if (car(sc->args) == sc->UNQUOTESP && ok_abbrev(cdr(sc->args))) {
               putstr(sc, ",@");
               sc->args = cadr(sc->args);
               s_goto(sc,OP_P0LIST);
          } else {
               putstr(sc, "(");
               s_save(sc,OP_P1LIST, cdr(sc->args), sc->NIL);
               sc->args = car(sc->args);
               s_goto(sc,OP_P0LIST);
          }

     case OP_P1LIST:
          if (is_pair(sc->args)) {
            s_save(sc,OP_P1LIST, cdr(sc->args), sc->NIL);
            putstr(sc, " ");
            sc->args = car(sc->args);
            s_goto(sc,OP_P0LIST);
          } else if(is_vector(sc->args)) {
            s_save(sc,OP_P1LIST,sc->NIL,sc->NIL);
            putstr(sc, " . ");
            s_goto(sc,OP_P0LIST);
          } else {
            if (sc->args != sc->NIL) {
              putstr(sc, " . ");
              printatom(sc, sc->args, sc->print_flag);
            }
            putstr(sc, ")");
            s_return(sc,sc->T);
          }
     case OP_PVECFROM: {
          int i=ivalue_unchecked(cdr(sc->args));
          pointer vec=car(sc->args);
          int len=ivalue_unchecked(vec);
          if(i==len) {
               putstr(sc," )");
               s_return(sc,sc->T);
          } else {
               pointer elem=vector_elem(vec,i);
               ivalue_unchecked(cdr(sc->args))=i+1;
               s_save(sc,OP_PVECFROM, sc->args, sc->NIL);
               sc->args=elem;
               putstr(sc," ");
               s_goto(sc,OP_P0LIST);
          }
     }

     default:
          sprintf(sc->strbuff, "%d: illegal operator", sc->op);
          Error_0(sc,sc->strbuff);

     }
     return sc->T;
}