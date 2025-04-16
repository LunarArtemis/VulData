static void file_pop(scheme *sc) {
 sc->nesting=sc->nesting_stack[sc->file_i];
 if(sc->file_i!=0) {
   port_close(sc,sc->loadport,port_input);
   sc->file_i--;
   sc->loadport->_object._port=sc->load_stack+sc->file_i;
   if(file_interactive(sc)) {
     putstr(sc,prompt);
   }
 }
}