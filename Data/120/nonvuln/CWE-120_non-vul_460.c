static gunichar basic_inchar(port *pt) {
  if(pt->kind & port_file) {
    int  c;

    c = fgetc(pt->rep.stdio.file);

    while (TRUE)
      {
        if (c == EOF) return EOF;

        if (c <= 0x7f)
            return (gunichar) c;

        /* Is this byte an invalid lead per RFC-3629? */
        if (c < 0xc2 || c > 0xf4)
          {
            /* Ignore invalid lead byte and get the next characer */
            c = fgetc(pt->rep.stdio.file);
          }
        else    /* Byte is valid lead */
          {
            unsigned char utf8[7];
            int  len;
            int  i;

            utf8[0] = c;    /* Save the lead byte */

            len = utf8_length[c & 0x3F];
            for (i = 1; i <= len; i++)
              {
                c = fgetc(pt->rep.stdio.file);

                /* Stop reading if this is not a continuation character */
                if ((c & 0xc0) != 0x80)
                    break;

                utf8[i] = c;
              }

            if (i > len)    /* Read the expected number of bytes? */
              {
                return g_utf8_get_char_validated ((char *) utf8,
                                                  sizeof(utf8));
              }

            /* Not enough continuation characters so ignore and restart */
          }
      } /* end of while (TRUE) */
  } else {
    gunichar c;
    int      len;

    while (TRUE)
    {
      /* Found NUL or at end of input buffer? */
      if (*pt->rep.string.curr == 0 ||
          pt->rep.string.curr == pt->rep.string.past_the_end) {
        return EOF;
      }

      len = pt->rep.string.past_the_end - pt->rep.string.curr;
      c = g_utf8_get_char_validated(pt->rep.string.curr, len);

      if (c >= 0)   /* Valid UTF-8 character? */
      {
        len = g_unichar_to_utf8(c, NULL);   /* Length of UTF-8 sequence */
        pt->rep.string.curr += len;
        return c;
      }

      /* Look for next valid UTF-8 character in buffer */
      pt->rep.string.curr = g_utf8_find_next_char(pt->rep.string.curr,
                                                  pt->rep.string.past_the_end);
    } /* end of while (TRUE) */
  }
}