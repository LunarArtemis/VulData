static pointer opexe_0(scheme *sc, enum scheme_opcodes op) {
     pointer x, y;

     switch (op) {
     case OP_LOAD:       /* load */
          if(file_interactive(sc)) {
               fprintf(sc->outport->_object._port->rep.stdio.file,
                       "Loading %s\n", strvalue(car(sc->args)));
          }
          if (!file_push(sc,strvalue(car(sc->args)))) {
               Error_1(sc,"unable to open", car(sc->args));
          }
          else
            { s_retbool(1); }

     case OP_T0LVL: /* top level */
          if(file_interactive(sc)) {
               putstr(sc,"\n");
          }
          sc->nesting=0;
          dump_stack_reset(sc);
          sc->envir = sc->global_env;
          sc->save_inport=sc->inport;
          sc->inport = sc->loadport;
          s_save(sc,OP_T0LVL, sc->NIL, sc->NIL);
          s_save(sc,OP_VALUEPRINT, sc->NIL, sc->NIL);
          s_save(sc,OP_T1LVL, sc->NIL, sc->NIL);
          if (file_interactive(sc)) {
              putstr(sc,prompt);
          }
          s_goto(sc,OP_READ_INTERNAL);

     case OP_T1LVL: /* top level */
          sc->code = sc->value;
          sc->inport=sc->save_inport;
          s_goto(sc,OP_EVAL);

     case OP_READ_INTERNAL:       /* internal read */
          sc->tok = token(sc);
          if(sc->tok==TOK_EOF) {
               if(sc->inport==sc->loadport) {
                    sc->args=sc->NIL;
                    s_goto(sc,OP_QUIT);
               } else {
                    s_return(sc,sc->EOF_OBJ);
               }
          }
          s_goto(sc,OP_RDSEXPR);

     case OP_GENSYM:
          s_return(sc, gensym(sc));

     case OP_VALUEPRINT: /* print evaluation result */
          /* OP_VALUEPRINT is always pushed, because when changing from
             non-interactive to interactive mode, it needs to be
             already on the stack */
       if(sc->tracing) {
         putstr(sc,"\nGives: ");
       }
       if(file_interactive(sc) || sc->print_output) {
         sc->print_flag = 1;
         sc->args = sc->value;
         s_goto(sc,OP_P0LIST);
       } else {
         s_return(sc,sc->value);
       }

     case OP_EVAL:       /* main part of evaluation */
#if USE_TRACING
       if(sc->tracing) {
         /*s_save(sc,OP_VALUEPRINT,sc->NIL,sc->NIL);*/
         s_save(sc,OP_REAL_EVAL,sc->args,sc->code);
         sc->args=sc->code;
         putstr(sc,"\nEval: ");
         s_goto(sc,OP_P0LIST);
       }
       /* fall through */
     case OP_REAL_EVAL:
#endif
          if (is_symbol(sc->code)) {    /* symbol */
               x=find_slot_in_env(sc,sc->envir,sc->code,1);
               if (x != sc->NIL) {
                    s_return(sc,slot_value_in_env(x));
               } else {
                    Error_1(sc,"eval: unbound variable:", sc->code);
               }
          } else if (is_pair(sc->code)) {
               if (is_syntax(x = car(sc->code))) {     /* SYNTAX */
                    sc->code = cdr(sc->code);
                    s_goto(sc,syntaxnum(x));
               } else {/* first, eval top element and eval arguments */
                    s_save(sc,OP_E0ARGS, sc->NIL, sc->code);
                    /* If no macros => s_save(sc,OP_E1ARGS, sc->NIL, cdr(sc->code));*/
                    sc->code = car(sc->code);
                    s_goto(sc,OP_EVAL);
               }
          } else {
               s_return(sc,sc->code);
          }

     case OP_E0ARGS:     /* eval arguments */
          if (is_macro(sc->value)) {    /* macro expansion */
               s_save(sc,OP_DOMACRO, sc->NIL, sc->NIL);
               sc->args = cons(sc,sc->code, sc->NIL);
               sc->code = sc->value;
               s_goto(sc,OP_APPLY);
          } else {
               sc->code = cdr(sc->code);
               s_goto(sc,OP_E1ARGS);
          }

     case OP_E1ARGS:     /* eval arguments */
          sc->args = cons(sc, sc->value, sc->args);
          if (is_pair(sc->code)) { /* continue */
               s_save(sc,OP_E1ARGS, sc->args, cdr(sc->code));
               sc->code = car(sc->code);
               sc->args = sc->NIL;
               s_goto(sc,OP_EVAL);
          } else {  /* end */
               sc->args = reverse_in_place(sc, sc->NIL, sc->args);
               sc->code = car(sc->args);
               sc->args = cdr(sc->args);
               s_goto(sc,OP_APPLY);
          }

#if USE_TRACING
     case OP_TRACING: {
       int tr=sc->tracing;
       sc->tracing=ivalue(car(sc->args));
       s_return(sc,mk_integer(sc,tr));
     }
#endif

     case OP_APPLY:      /* apply 'code' to 'args' */
#if USE_TRACING
       if(sc->tracing) {
         s_save(sc,OP_REAL_APPLY,sc->args,sc->code);
         sc->print_flag = 1;
         /*         sc->args=cons(sc,sc->code,sc->args);*/
         putstr(sc,"\nApply to: ");
         s_goto(sc,OP_P0LIST);
       }
       /* fall through */
     case OP_REAL_APPLY:
#endif
          if (is_proc(sc->code)) {
               s_goto(sc,procnum(sc->code));   /* PROCEDURE */
          } else if (is_foreign(sc->code))
          {
               /* Keep nested calls from GC'ing the arglist */
               push_recent_alloc(sc,sc->args,sc->NIL);
               sc->foreign_error = sc->NIL;
               x=sc->code->_object._ff(sc,sc->args);
               if (sc->foreign_error == sc->NIL) {
                   s_return(sc,x);
               } else {
                   x = sc->foreign_error;
                   sc->foreign_error = sc->NIL;
                   Error_1 (sc, string_value (car (x)), cdr (x));
               }
          } else if (is_closure(sc->code) || is_macro(sc->code)
                     || is_promise(sc->code)) { /* CLOSURE */
            /* Should not accept promise */
               /* make environment */
               new_frame_in_env(sc, closure_env(sc->code));
               for (x = car(closure_code(sc->code)), y = sc->args;
                    is_pair(x); x = cdr(x), y = cdr(y)) {
                    if (y == sc->NIL) {
                         Error_0(sc,"not enough arguments");
                    } else {
                         new_slot_in_env(sc, car(x), car(y));
                    }
               }
               if (x == sc->NIL) {
                    /*--
                     * if (y != sc->NIL) {
                     *   Error_0(sc,"too many arguments");
                     * }
                     */
               } else if (is_symbol(x))
                    new_slot_in_env(sc, x, y);
               else {
                    Error_1(sc,"syntax error in closure: not a symbol:", x);
               }
               sc->code = cdr(closure_code(sc->code));
               sc->args = sc->NIL;
               s_goto(sc,OP_BEGIN);
          } else if (is_continuation(sc->code)) { /* CONTINUATION */
               sc->dump = cont_dump(sc->code);
               s_return(sc,sc->args != sc->NIL ? car(sc->args) : sc->NIL);
          } else {
               Error_0(sc,"illegal function");
          }

     case OP_DOMACRO:    /* do macro */
          sc->code = sc->value;
          s_goto(sc,OP_EVAL);

     case OP_LAMBDA:     /* lambda */
          s_return(sc,mk_closure(sc, sc->code, sc->envir));

     case OP_MKCLOSURE: /* make-closure */
       x=car(sc->args);
       if(car(x)==sc->LAMBDA) {
         x=cdr(x);
       }
       if(cdr(sc->args)==sc->NIL) {
         y=sc->envir;
       } else {
         y=cadr(sc->args);
       }
       s_return(sc,mk_closure(sc, x, y));

     case OP_QUOTE:      /* quote */
          x=car(sc->code);
          s_return(sc,car(sc->code));

     case OP_DEF0:  /* define */
          if(is_immutable(car(sc->code)))
                Error_1(sc,"define: unable to alter immutable", car(sc->code));
          if (is_pair(car(sc->code))) {
               x = caar(sc->code);
               sc->code = cons(sc, sc->LAMBDA, cons(sc, cdar(sc->code), cdr(sc->code)));
          } else {
               x = car(sc->code);
               sc->code = cadr(sc->code);
          }
          if (!is_symbol(x)) {
               Error_0(sc,"variable is not a symbol");
          }
          s_save(sc,OP_DEF1, sc->NIL, x);
          s_goto(sc,OP_EVAL);

     case OP_DEF1:  /* define */
          x=find_slot_in_env(sc,sc->envir,sc->code,0);
          if (x != sc->NIL) {
               set_slot_in_env(sc, x, sc->value);
          } else {
               new_slot_in_env(sc, sc->code, sc->value);
          }
          s_return(sc,sc->code);


     case OP_DEFP:  /* defined? */
          x=sc->envir;
          if(cdr(sc->args)!=sc->NIL) {
               x=cadr(sc->args);
          }
          s_retbool(find_slot_in_env(sc,x,car(sc->args),1)!=sc->NIL);

     case OP_SET0:       /* set! */
          if(is_immutable(car(sc->code)))
                Error_1(sc,"set!: unable to alter immutable variable", car(sc->code));
          s_save(sc,OP_SET1, sc->NIL, car(sc->code));
          sc->code = cadr(sc->code);
          s_goto(sc,OP_EVAL);

     case OP_SET1:       /* set! */
          y=find_slot_in_env(sc,sc->envir,sc->code,1);
          if (y != sc->NIL) {
             set_slot_in_env(sc, y, sc->value);
             s_return(sc,sc->value);
          } else {
             Error_1(sc,"set!: unbound variable:", sc->code);
          }

     case OP_BEGIN:      /* begin */
          if (!is_pair(sc->code)) {
               s_return(sc,sc->code);
          }
          if (cdr(sc->code) != sc->NIL) {
               s_save(sc,OP_BEGIN, sc->NIL, cdr(sc->code));
          }
          sc->code = car(sc->code);
          s_goto(sc,OP_EVAL);

     case OP_IF0:        /* if */
          s_save(sc,OP_IF1, sc->NIL, cdr(sc->code));
          sc->code = car(sc->code);
          s_goto(sc,OP_EVAL);

     case OP_IF1:        /* if */
          if (is_true(sc->value))
               sc->code = car(sc->code);
          else
               sc->code = cadr(sc->code);  /* (if #f 1) ==> () because
                               * car(sc->NIL) = sc->NIL */
          s_goto(sc,OP_EVAL);

     case OP_LET0:       /* let */
          sc->args = sc->NIL;
          sc->value = sc->code;
          sc->code = is_symbol(car(sc->code)) ? cadr(sc->code) : car(sc->code);
          s_goto(sc,OP_LET1);

     case OP_LET1:       /* let (calculate parameters) */
          sc->args = cons(sc, sc->value, sc->args);
          if (is_pair(sc->code)) { /* continue */
               if (!is_pair(car(sc->code)) || !is_pair(cdar(sc->code))) {
                    Error_1(sc, "Bad syntax of binding spec in let :", car(sc->code));
               }
               s_save(sc,OP_LET1, sc->args, cdr(sc->code));
               sc->code = cadar(sc->code);
               sc->args = sc->NIL;
               s_goto(sc,OP_EVAL);
          } else {  /* end */
               sc->args = reverse_in_place(sc, sc->NIL, sc->args);
               sc->code = car(sc->args);
               sc->args = cdr(sc->args);
               s_goto(sc,OP_LET2);
          }

     case OP_LET2:       /* let */
          new_frame_in_env(sc, sc->envir);
          for (x = is_symbol(car(sc->code)) ? cadr(sc->code) : car(sc->code), y = sc->args;
               y != sc->NIL; x = cdr(x), y = cdr(y)) {
               new_slot_in_env(sc, caar(x), car(y));
          }
          if (is_symbol(car(sc->code))) {    /* named let */
               for (x = cadr(sc->code), sc->args = sc->NIL; x != sc->NIL; x = cdr(x)) {
                    if (!is_pair(x))
                        Error_1(sc, "Bad syntax of binding in let :", x);
                    if (!is_list(sc, car(x)))
                        Error_1(sc, "Bad syntax of binding in let :", car(x));
                    sc->args = cons(sc, caar(x), sc->args);
               }
               x = mk_closure(sc, cons(sc, reverse_in_place(sc, sc->NIL, sc->args), cddr(sc->code)), sc->envir);
               new_slot_in_env(sc, car(sc->code), x);
               sc->code = cddr(sc->code);
               sc->args = sc->NIL;
          } else {
               sc->code = cdr(sc->code);
               sc->args = sc->NIL;
          }
          s_goto(sc,OP_BEGIN);

     case OP_LET0AST:    /* let* */
          if (car(sc->code) == sc->NIL) {
               new_frame_in_env(sc, sc->envir);
               sc->code = cdr(sc->code);
               s_goto(sc,OP_BEGIN);
          }
          if(!is_pair(car(sc->code)) || !is_pair(caar(sc->code)) || !is_pair(cdaar(sc->code))) {
               Error_1(sc,"Bad syntax of binding spec in let* :",car(sc->code));
          }
          s_save(sc,OP_LET1AST, cdr(sc->code), car(sc->code));
          sc->code = cadaar(sc->code);
          s_goto(sc,OP_EVAL);

     case OP_LET1AST:    /* let* (make new frame) */
          new_frame_in_env(sc, sc->envir);
          s_goto(sc,OP_LET2AST);

     case OP_LET2AST:    /* let* (calculate parameters) */
          new_slot_in_env(sc, caar(sc->code), sc->value);
          sc->code = cdr(sc->code);
          if (is_pair(sc->code)) { /* continue */
               s_save(sc,OP_LET2AST, sc->args, sc->code);
               sc->code = cadar(sc->code);
               sc->args = sc->NIL;
               s_goto(sc,OP_EVAL);
          } else {  /* end */
               sc->code = sc->args;
               sc->args = sc->NIL;
               s_goto(sc,OP_BEGIN);
          }
     default:
          sprintf(sc->strbuff, "%d: illegal operator", sc->op);
          Error_0(sc,sc->strbuff);
     }
     return sc->T;
}