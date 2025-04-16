SCHEME_EXPORT void scheme_set_input_port_file(scheme *sc, FILE *fin) {
  sc->inport=port_from_file(sc,fin,port_input);
}