static void assign_proc(scheme *sc, enum scheme_opcodes op, char *name) {
     pointer x, y;

     x = mk_symbol(sc, name);
     y = mk_proc(sc,op);
     new_slot_in_env(sc, x, y);
}