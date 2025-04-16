static port *port_rep_from_file(scheme *sc, FILE *f, int prop) {
  char *rw;
  port *pt;
  pt=(port*)sc->malloc(sizeof(port));
  if(pt==0) {
    return 0;
  }
  if(prop==(port_input|port_output)) {
    rw="a+";
  } else if(prop==port_output) {
    rw="w";
  } else {
    rw="r";
  }
  pt->kind=port_file|prop;
  pt->rep.stdio.file=f;
  pt->rep.stdio.closeit=0;
  return pt;
}