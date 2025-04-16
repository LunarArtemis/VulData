static void finalize_cell(scheme *sc, pointer a) {
  if(is_string(a)) {
    sc->free(strvalue(a));
  } else if(is_port(a)) {
    if(a->_object._port->kind&port_file
       && a->_object._port->rep.stdio.closeit) {
      port_close(sc,a,port_input|port_output);
    }
    sc->free(a->_object._port);
  }
}