static port *port_rep_from_string(scheme *sc, char *start, char *past_the_end, int prop) {
  port *pt;
  pt=(port*)sc->malloc(sizeof(port));
  if(pt==0) {
    return 0;
  }
  pt->kind=port_string|prop;
  pt->rep.string.start=start;
  pt->rep.string.curr=start;
  pt->rep.string.past_the_end=past_the_end;
  return pt;
}