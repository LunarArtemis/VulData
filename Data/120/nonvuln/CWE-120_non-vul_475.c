static void printslashstring(scheme *sc, char *p, int len) {
  int i;
  gunichar c;
  char *s=(char*)p;

  putcharacter(sc,'"');
  for (i=0; i<len; i++) {
    c = g_utf8_get_char(s);
    /* Is a check for a value of 0xff still valid in UTF8?? ~~~~~ */
    if(c==0xff || c=='"' || c<' ' || c=='\\') {
      putcharacter(sc,'\\');
      switch(c) {
      case '"':
        putcharacter(sc,'"');
        break;
      case '\n':
        putcharacter(sc,'n');
        break;
      case '\t':
        putcharacter(sc,'t');
        break;
      case '\r':
        putcharacter(sc,'r');
        break;
      case '\\':
        putcharacter(sc,'\\');
        break;
      default: {
          /* This still needs work ~~~~~ */
          int d=c/16;
          putcharacter(sc,'x');
          if(d<10) {
            putcharacter(sc,d+'0');
          } else {
            putcharacter(sc,d-10+'A');
          }
          d=c%16;
          if(d<10) {
            putcharacter(sc,d+'0');
          } else {
            putcharacter(sc,d-10+'A');
          }
        }
      }
    } else {
      putcharacter(sc,c);
    }
    s = g_utf8_next_char(s);
  }
  putcharacter(sc,'"');
}