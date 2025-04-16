INTERFACE void putcharacter(scheme *sc, gunichar c) {
  char utf8[7];

  (void)g_unichar_to_utf8(c, utf8);
  putchars(sc, utf8, 1);
}