static pointer readstrexp(scheme *sc) {
  char *p = sc->strbuff;
  gunichar c;
  int c1=0;
  int len;
  enum { st_ok, st_bsl, st_x1, st_x2, st_oct1, st_oct2 } state=st_ok;

  for (;;) {
    c=inchar(sc);
    if(c==EOF || p-sc->strbuff>sizeof(sc->strbuff)-1) {
      return sc->F;
    }
    switch(state) {
    case st_ok:
      switch(c) {
      case '\\':
        state=st_bsl;
        break;
      case '"':
        *p=0;
        return mk_counted_string(sc,sc->strbuff,
                                 g_utf8_strlen(sc->strbuff, sizeof(sc->strbuff)));
      default:
        len = g_unichar_to_utf8(c, p);
        p += len;
        break;
      }
      break;
    case st_bsl:
      switch(c) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
        state=st_oct1;
        c1=g_unichar_digit_value(c);
        break;
      case 'x':
      case 'X':
        state=st_x1;
        c1=0;
        break;
      case 'n':
        *p++='\n';
        state=st_ok;
        break;
      case 't':
        *p++='\t';
        state=st_ok;
        break;
      case 'r':
        *p++='\r';
        state=st_ok;
        break;
      case '"':
        *p++='"';
        state=st_ok;
        break;
      default:
        len = g_unichar_to_utf8(c, p);
        p += len;
        state=st_ok;
        break;
      }
      break;
    case st_x1:
    case st_x2:
      if (!g_unichar_isxdigit(c))
         return sc->F;
      c1=(c1<<4)+g_unichar_xdigit_value(c);
      if(state==st_x1)
        state=st_x2;
      else {
        *p++=c1;
        state=st_ok;
      }
      break;
    case st_oct1:   /* State when handling second octal digit */
    case st_oct2:   /* State when handling third octal digit */
      if (!g_unichar_isdigit(c) || g_unichar_digit_value(c) > 7)
      {
        *p++=c1;
        backchar(sc, c);
        state=st_ok;
      }
      else
      {
        /* Is value of three character octal too big for a byte? */
        if (state==st_oct2 && c1 >= 32)
          return sc->F;

        c1=(c1<<3)+g_unichar_digit_value(c);

        if (state == st_oct1)
          state=st_oct2;
        else
        {
          *p++=c1;
          state=st_ok;
        }
      }
      break;
    }
  }
}