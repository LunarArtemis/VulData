static void printatom(scheme *sc, pointer l, int f) {
  char *p;
  int len;
  atom2str(sc,l,f,&p,&len);
  putchars(sc,p,len);
}