static void port_close(scheme *sc, pointer p, int flag) {
  port *pt=p->_object._port;
  pt->kind&=~flag;
  if((pt->kind & (port_input|port_output))==0) {
    if(pt->kind&port_file) {
      fclose(pt->rep.stdio.file);
    }
    pt->kind=port_free;
  }
}