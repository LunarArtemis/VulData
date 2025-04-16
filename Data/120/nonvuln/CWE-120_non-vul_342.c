void scheme_set_output_port_string(scheme *sc, char *start, char *past_the_end) {
  sc->outport=port_from_string(sc,start,past_the_end,port_output);
}