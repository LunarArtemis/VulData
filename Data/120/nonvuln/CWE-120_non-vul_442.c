static void Eval_Cycle(scheme *sc, enum scheme_opcodes op) {
  int count=0;
  int old_op;

  sc->op = op;
  for (;;) {
    op_code_info *pcd=dispatch_table+sc->op;
    if (pcd->name!=0) { /* if built-in function, check arguments */
      char msg[512];
      int ok=1;
      int n=list_length(sc,sc->args);

      /* Check number of arguments */
      if(n<pcd->min_arity) {
        ok=0;
        sprintf(msg,"%s: needs%s %d argument(s)",
                pcd->name,
                pcd->min_arity==pcd->max_arity?"":" at least",
                pcd->min_arity);
      }
      if(ok && n>pcd->max_arity) {
        ok=0;
        sprintf(msg,"%s: needs%s %d argument(s)",
                pcd->name,
                pcd->min_arity==pcd->max_arity?"":" at most",
                pcd->max_arity);
      }
      if(ok) {
        if(pcd->arg_tests_encoding!=0) {
          int i=0;
          int j;
          const char *t=pcd->arg_tests_encoding;
          pointer arglist=sc->args;
          do {
            pointer arg=car(arglist);
            j=(int)t[0];
            if(j==TST_INPORT[0]) {
              if(!is_inport(arg)) break;
            } else if(j==TST_OUTPORT[0]) {
              if(!is_outport(arg)) break;
            } else if(j==TST_LIST[0]) {
              if(arg!=sc->NIL && !is_pair(arg)) break;
            } else {
              if(!tests[j].fct(arg)) break;
            }

            if(t[1]!=0) {/* last test is replicated as necessary */
              t++;
            }
            arglist=cdr(arglist);
            i++;
          } while(i<n);
          if(i<n) {
            ok=0;
            sprintf(msg,"%s: argument %d must be: %s",
                    pcd->name,
                    i+1,
                    tests[j].kind);
          }
        }
      }
      if(!ok) {
        if(_Error_1(sc,msg,0)==sc->NIL) {
          return;
        }
        pcd=dispatch_table+sc->op;
      }
    }
    ok_to_freely_gc(sc);
    old_op=sc->op;
    if (pcd->func(sc, (enum scheme_opcodes)sc->op) == sc->NIL) {
      return;
    }
    if(sc->no_memory) {
      fprintf(stderr,"No memory!\n");
      return;
    }
    count++;
  }
}