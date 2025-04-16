static void assign_syntax(scheme *sc, char *name) {
     pointer x;

     x = oblist_add_by_name(sc, name);
     typeflag(x) |= T_SYNTAX;
}