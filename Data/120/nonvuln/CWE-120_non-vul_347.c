static int file_interactive(scheme *sc) {
 return sc->file_i==0 && sc->load_stack[0].rep.stdio.file==stdin
     && sc->inport->_object._port->kind&port_file;
}