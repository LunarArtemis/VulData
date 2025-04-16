void scheme_set_input_port_string(scheme *sc, char *start, char *past_the_end) {
  sc->inport=port_from_string(sc,start,past_the_end,port_input);
}