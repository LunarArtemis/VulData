static pointer opexe_3(scheme *sc, enum scheme_opcodes op) {
     pointer x;
     num v;
     int (*comp_func)(num,num)=0;

     switch (op) {
     case OP_NOT:        /* not */
          s_retbool(is_false(car(sc->args)));
     case OP_BOOLP:       /* boolean? */
          s_retbool(car(sc->args) == sc->F || car(sc->args) == sc->T);
     case OP_EOFOBJP:       /* boolean? */
          s_retbool(car(sc->args) == sc->EOF_OBJ);
     case OP_NULLP:       /* null? */
          s_retbool(car(sc->args) == sc->NIL);
     case OP_NUMEQ:      /* = */
     case OP_LESS:       /* < */
     case OP_GRE:        /* > */
     case OP_LEQ:        /* <= */
     case OP_GEQ:        /* >= */
          switch(op) {
               case OP_NUMEQ: comp_func=num_eq; break;
               case OP_LESS:  comp_func=num_lt; break;
               case OP_GRE:   comp_func=num_gt; break;
               case OP_LEQ:   comp_func=num_le; break;
               case OP_GEQ:   comp_func=num_ge; break;
               default:       break;  /* Quiet the compiler */
          }
          x=sc->args;
          v=nvalue(car(x));
          x=cdr(x);

          for (; x != sc->NIL; x = cdr(x)) {
               if(!comp_func(v,nvalue(car(x)))) {
                    s_retbool(0);
               }
               v=nvalue(car(x));
          }
          s_retbool(1);
     case OP_SYMBOLP:     /* symbol? */
          s_retbool(is_symbol(car(sc->args)));
     case OP_NUMBERP:     /* number? */
          s_retbool(is_number(car(sc->args)));
     case OP_STRINGP:     /* string? */
          s_retbool(is_string(car(sc->args)));
     case OP_INTEGERP:     /* integer? */
          s_retbool(is_number(car(sc->args)) && is_integer(car(sc->args)));
     case OP_REALP:     /* real? */
          s_retbool(is_number(car(sc->args))); /* All numbers are real */
     case OP_CHARP:     /* char? */
          s_retbool(is_character(car(sc->args)));
#if USE_CHAR_CLASSIFIERS
     case OP_CHARAP:     /* char-alphabetic? */
          s_retbool(Cisalpha(ivalue(car(sc->args))));
     case OP_CHARNP:     /* char-numeric? */
          s_retbool(Cisdigit(ivalue(car(sc->args))));
     case OP_CHARWP:     /* char-whitespace? */
          s_retbool(Cisspace(ivalue(car(sc->args))));
     case OP_CHARUP:     /* char-upper-case? */
          s_retbool(Cisupper(ivalue(car(sc->args))));
     case OP_CHARLP:     /* char-lower-case? */
          s_retbool(Cislower(ivalue(car(sc->args))));
#endif
     case OP_PORTP:     /* port? */
          s_retbool(is_port(car(sc->args)));
     case OP_INPORTP:     /* input-port? */
          s_retbool(is_inport(car(sc->args)));
     case OP_OUTPORTP:     /* output-port? */
          s_retbool(is_outport(car(sc->args)));
     case OP_PROCP:       /* procedure? */
          /*--
              * continuation should be procedure by the example
              * (call-with-current-continuation procedure?) ==> #t
                 * in R^3 report sec. 6.9
              */
          s_retbool(is_proc(car(sc->args)) || is_closure(car(sc->args))
                 || is_continuation(car(sc->args)) || is_foreign(car(sc->args)));
     case OP_PAIRP:       /* pair? */
          s_retbool(is_pair(car(sc->args)));
     case OP_LISTP:       /* list? */
          s_retbool(list_length(sc, car(sc->args)) >= 0);

     case OP_ENVP:        /* environment? */
          s_retbool(is_environment(car(sc->args)));
     case OP_VECTORP:     /* vector? */
          s_retbool(is_vector(car(sc->args)));
     case OP_EQ:         /* eq? */
          s_retbool(car(sc->args) == cadr(sc->args));
     case OP_EQV:        /* eqv? */
          s_retbool(eqv(car(sc->args), cadr(sc->args)));
     default:
          sprintf(sc->strbuff, "%d: illegal operator", sc->op);
          Error_0(sc,sc->strbuff);
     }
     return sc->T;
}