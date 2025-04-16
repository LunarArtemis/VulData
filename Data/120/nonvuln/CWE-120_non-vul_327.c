static char *store_string(scheme *sc, int char_cnt,
                          const char *str, gunichar fill) {
     int  len;
     int  i;
     gchar utf8[7];
     gchar *q;
     gchar *q2;

     if(str!=0) {
       q2 = g_utf8_offset_to_pointer(str, (long)char_cnt);
       (void)g_utf8_validate(str, -1, (const gchar **)&q);
       if (q <= q2)
          len = q - str;
       else
          len = q2 - str;
       q=(gchar*)sc->malloc(len+1);
     } else {
       len = g_unichar_to_utf8(fill, utf8);
       q=(gchar*)sc->malloc(char_cnt*len+1);
     }

     if(q==0) {
       sc->no_memory=1;
       return sc->strbuff;
     }
     if(str!=0) {
       memcpy(q, str, len);
       q[len]=0;
     } else {
       q2 = q;
       for (i = 0; i < char_cnt; ++i)
       {
         memcpy(q2, utf8, len);
         q2 += len;
       }
       *q2=0;
     }
     return (q);
}