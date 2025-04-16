static pointer opexe_6(scheme *sc, enum scheme_opcodes op) {
     pointer x, y;
     long v;

     switch (op) {
     case OP_LIST_LENGTH:     /* length */   /* a.k */
          v=list_length(sc,car(sc->args));
          if(v<0) {
               Error_1(sc,"length: not a list:",car(sc->args));
          }
          s_return(sc,mk_integer(sc, v));

     case OP_ASSQ:       /* assq */     /* a.k */
          x = car(sc->args);
          for (y = cadr(sc->args); is_pair(y); y = cdr(y)) {
               if (!is_pair(car(y))) {
                    Error_0(sc,"unable to handle non pair element");
               }
               if (x == caar(y))
                    break;
          }
          if (is_pair(y)) {
               s_return(sc,car(y));
          } else {
               s_return(sc,sc->F);
          }


     case OP_GET_CLOSURE:     /* get-closure-code */   /* a.k */
          sc->args = car(sc->args);
          if (sc->args == sc->NIL) {
               s_return(sc,sc->F);
          } else if (is_closure(sc->args)) {
               s_return(sc,cons(sc, sc->LAMBDA, closure_code(sc->value)));
          } else if (is_macro(sc->args)) {
               s_return(sc,cons(sc, sc->LAMBDA, closure_code(sc->value)));
          } else {
               s_return(sc,sc->F);
          }
     case OP_CLOSUREP:        /* closure? */
          /*
           * Note, macro object is also a closure.
           * Therefore, (closure? <#MACRO>) ==> #t
           */
          s_retbool(is_closure(car(sc->args)));
     case OP_MACROP:          /* macro? */
          s_retbool(is_macro(car(sc->args)));
     default:
          sprintf(sc->strbuff, "%d: illegal operator", sc->op);
          Error_0(sc,sc->strbuff);
     }
     return sc->T; /* NOTREACHED */
}