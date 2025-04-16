static int file_push(scheme *sc, const char *fname) {
  FILE *fin=fopen(fname,"rb");
  if(fin!=0) {
    sc->file_i++;
    sc->load_stack[sc->file_i].kind=port_file|port_input;
    sc->load_stack[sc->file_i].rep.stdio.file=fin;
    sc->load_stack[sc->file_i].rep.stdio.closeit=1;
    sc->nesting_stack[sc->file_i]=0;
    sc->loadport->_object._port=sc->load_stack+sc->file_i;
  }
  return fin!=0;
}