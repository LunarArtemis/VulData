static pointer port_from_string(scheme *sc, char *start, char *past_the_end, int prop) {
  port *pt;
  pt=port_rep_from_string(sc,start,past_the_end,prop);
  if(pt==0) {
    return sc->NIL;
  }
  return mk_port(sc,pt);
}