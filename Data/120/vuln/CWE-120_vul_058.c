static char *readstr_upto(scheme *sc, char *delim) {
  char *p = sc->strbuff;
  gunichar c = 0;
  gunichar c_prev = 0;
  int  len = 0;

#if 0
  while (!is_one_of(delim, (*p++ = inchar(sc))))
      ;
  if(p==sc->strbuff+2 && p[-2]=='\\') {
    *p=0;
  } else {
    backchar(sc,p[-1]);
    *--p = '\0';
  }
#else
  do {
    c_prev = c;
    c = inchar(sc);
    len = g_unichar_to_utf8(c, p);
    p += len;
  } while (c && !is_one_of(delim, c));

  if(p==sc->strbuff+2 && c_prev=='\\')
    *p = '\0';
  else
  {
    backchar(sc,c);    /* put back the delimiter */
    p[-len] = '\0';
  }
#endif
  return sc->strbuff;
}