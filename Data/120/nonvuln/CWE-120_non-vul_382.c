SCHEME_EXPORT void scheme_set_output_port_file(scheme *sc, FILE *fout) {
  sc->outport=port_from_file(sc,fout,port_output);
}