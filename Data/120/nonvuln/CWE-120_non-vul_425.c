static gunichar inchar(scheme *sc) {
  gunichar c;
  port *pt;
 again:
  pt=sc->inport->_object._port;
  if(pt->kind&port_file)
  {
    if (sc->bc_flag)
      c = sc->backchar[--sc->bc_flag];
    else
      c=basic_inchar(pt);
  }
  else
    c=basic_inchar(pt);
  if(c==EOF && sc->inport==sc->loadport && sc->file_i!=0) {
    file_pop(sc);
    if(sc->nesting!=0) {
      return EOF;
    } else {
      return '\n';
    }
    goto again;
  }
  return c;
}