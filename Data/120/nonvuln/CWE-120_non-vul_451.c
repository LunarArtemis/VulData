static pointer mk_port(scheme *sc, port *p) {
  pointer x = get_cell(sc, sc->NIL, sc->NIL);

  typeflag(x) = T_PORT|T_ATOM;
  x->_object._port=p;
  return (x);
}