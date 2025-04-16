static pointer port_from_filename(scheme *sc, const char *fn, int prop) {
  port *pt;
  pt=port_rep_from_filename(sc,fn,prop);
  if(pt==0) {
    return sc->NIL;
  }
  return mk_port(sc,pt);
}