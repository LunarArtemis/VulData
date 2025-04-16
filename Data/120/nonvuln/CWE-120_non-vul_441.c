static void backchar(scheme *sc, gunichar c) {
  port *pt;
  gint  charlen;

  if(c==EOF) return;
  charlen = g_unichar_to_utf8(c, NULL);
  pt=sc->inport->_object._port;
  if(pt->kind&port_file) {
    if (sc->bc_flag < 2)
      sc->backchar[sc->bc_flag++] = c;
  } else {
    if(pt->rep.string.curr!=pt->rep.string.start) {
      if(pt->rep.string.curr-pt->rep.string.start >= charlen)
        pt->rep.string.curr -= charlen;
      else
        pt->rep.string.curr = pt->rep.string.start;
    }
  }
}