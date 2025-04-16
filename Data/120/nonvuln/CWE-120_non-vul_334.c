int scheme_init_custom_alloc(scheme *sc, func_alloc malloc, func_dealloc free) {
  int i, n=sizeof(dispatch_table)/sizeof(dispatch_table[0]);
  pointer x;

  num_zero.is_fixnum=1;
  num_zero.value.ivalue=0;
  num_one.is_fixnum=1;
  num_one.value.ivalue=1;

#if USE_INTERFACE
  sc->vptr=&vtbl;
#endif
  sc->gensym_cnt=0;
  sc->malloc=malloc;
  sc->free=free;
  sc->last_cell_seg = -1;
  sc->sink = &sc->_sink;
  sc->NIL = &sc->_NIL;
  sc->T = &sc->_HASHT;
  sc->F = &sc->_HASHF;
  sc->EOF_OBJ=&sc->_EOF_OBJ;
  sc->free_cell = &sc->_NIL;
  sc->fcells = 0;
  sc->no_memory=0;
  sc->inport=sc->NIL;
  sc->outport=sc->NIL;
  sc->save_inport=sc->NIL;
  sc->loadport=sc->NIL;
  sc->nesting=0;
  sc->interactive_repl=0;
  sc->print_output=0;

  if (alloc_cellseg(sc,FIRST_CELLSEGS) != FIRST_CELLSEGS) {
    sc->no_memory=1;
    return 0;
  }
  sc->gc_verbose = 0;
  dump_stack_initialize(sc);
  sc->code = sc->NIL;
  sc->tracing=0;
  sc->bc_flag = 0;

  /* init sc->NIL */
  typeflag(sc->NIL) = (T_ATOM | MARK);
  car(sc->NIL) = cdr(sc->NIL) = sc->NIL;
  /* init T */
  typeflag(sc->T) = (T_ATOM | MARK);
  car(sc->T) = cdr(sc->T) = sc->T;
  /* init F */
  typeflag(sc->F) = (T_ATOM | MARK);
  car(sc->F) = cdr(sc->F) = sc->F;
  /* init sink */
  typeflag(sc->sink) = (T_PAIR | MARK);
  car(sc->sink) = sc->NIL;

  sc->oblist = oblist_initial_value(sc);
  /* init global_env */
  new_frame_in_env(sc, sc->NIL);
  sc->global_env = sc->envir;
  /* init else */
  x = mk_symbol(sc,"else");
  new_slot_in_env(sc, x, sc->T);

  assign_syntax(sc, "lambda");
  assign_syntax(sc, "quote");
  assign_syntax(sc, "define");
  assign_syntax(sc, "if");
  assign_syntax(sc, "begin");
  assign_syntax(sc, "set!");
  assign_syntax(sc, "let");
  assign_syntax(sc, "let*");
  assign_syntax(sc, "letrec");
  assign_syntax(sc, "cond");
  assign_syntax(sc, "delay");
  assign_syntax(sc, "and");
  assign_syntax(sc, "or");
  assign_syntax(sc, "cons-stream");
  assign_syntax(sc, "macro");
  assign_syntax(sc, "case");

  for(i=0; i<n; i++) {
    if(dispatch_table[i].name!=0) {
      assign_proc(sc, (enum scheme_opcodes)i, dispatch_table[i].name);
    }
  }

  /* initialization of global pointers to special symbols */
  sc->LAMBDA = mk_symbol(sc, "lambda");
  sc->QUOTE = mk_symbol(sc, "quote");
  sc->QQUOTE = mk_symbol(sc, "quasiquote");
  sc->UNQUOTE = mk_symbol(sc, "unquote");
  sc->UNQUOTESP = mk_symbol(sc, "unquote-splicing");
  sc->FEED_TO = mk_symbol(sc, "=>");
  sc->COLON_HOOK = mk_symbol(sc,"*colon-hook*");
  sc->ERROR_HOOK = mk_symbol(sc, "*error-hook*");
  sc->SHARP_HOOK = mk_symbol(sc, "*sharp-hook*");

  return !sc->no_memory;
}