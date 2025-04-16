static INLINE void skipspace(scheme *sc) {
     gunichar c;
     while (g_unichar_isspace(c=inchar(sc)))
          ;
     if(c!=EOF) {
          backchar(sc,c);
     }
}