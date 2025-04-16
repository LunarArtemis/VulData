static void putchars(scheme *sc, const char *chars, int char_cnt) {
  int   free_bytes;     /* Space remaining in buffer (in bytes) */
  int   l;
  port *pt=sc->outport->_object._port;

  if (char_cnt <= 0)
      return;

  /* Get length of 'chars' in bytes */
  char_cnt = g_utf8_offset_to_pointer(chars, (long)char_cnt) - chars;

  if(pt->kind&port_file) {
#if STANDALONE
      fwrite(chars,1,char_cnt,pt->rep.stdio.file);
      fflush(pt->rep.stdio.file);
#else
      /* If output is still directed to stdout (the default) it should be    */
      /* safe to redirect it to the registered output routine. */
      if (pt->rep.stdio.file == stdout)
           ts_output_string (TS_OUTPUT_NORMAL, chars, char_cnt);
      else {
        fwrite(chars,1,char_cnt,pt->rep.stdio.file);
        fflush(pt->rep.stdio.file);
      }
#endif
  } else {
    free_bytes = pt->rep.string.past_the_end - pt->rep.string.curr;
    if (free_bytes > 0)
    {
       l = min(char_cnt, free_bytes);
       memcpy(pt->rep.string.curr, chars, l);
       pt->rep.string.curr += l;
    }
  }
}