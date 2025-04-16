static port *port_rep_from_filename(scheme *sc, const char *fn, int prop) {
  FILE *f;
  char *rw;
  port *pt;
  if(prop==(port_input|port_output)) {
    rw="a+b";
  } else if(prop==port_output) {
    rw="wb";
  } else {
    rw="rb";
  }
  f=fopen(fn,rw);
  if(f==0) {
    return 0;
  }
  pt=port_rep_from_file(sc,f,prop);
  pt->rep.stdio.closeit=1;
  return pt;
}