double asn1_get_real(const guint8 *real_ptr, gint len) {
  guint8 octet;
  const guint8 *p;
  guint8 *buf;
  double val = 0;

  /* 8.5.2    If the real value is the value zero,
   *          there shall be no contents octets in the encoding.
   */
  if (len < 1) return val;

  octet = real_ptr[0];
  p = real_ptr + 1;
  len -= 1;
  if (octet & 0x80) {  /* binary encoding */
    int i;
    gboolean Eneg;
    gint8 S; /* Sign */
    guint8 B; /* Base */
    guint8 F; /* scaling Factor */
    gint32 E = 0; /* Exponent (supported max 3 octets/24 bit) */
    guint64 N = 0; /* N (supported max 8 octets/64 bit) */

    guint8 lenE, lenN;

    if(octet & 0x40) S = -1; else S = 1;
    switch(octet & 0x30) {
      case 0x00: B = 2; break;
      case 0x10: B = 8; break;
      case 0x20: B = 16; break;
      case 0x30: /* Reserved */
      default:
        /* TODO Add some warning in tree about reserved value for Base */
        return 0;
    }
    F = (octet & 0x0c) >> 2;

    /* 8.5.6.4 Exponent length */
    lenE = (octet & 0x3) + 1;
    if(lenE == 4)
    {
      /* we can't handle exponents > 24 bits */
      /* TODO Next octet(s) define length of exponent */
      DISSECTOR_ASSERT_NOT_REACHED();
    }

    Eneg = (*p) & 0x80 ? TRUE : FALSE;
    for (i = 0; i < lenE; i++) {
      if(Eneg) {
        /* 2's complement: inverse bits */
        E = (E<<8) | ((guint8) ~(*p));
      } else {
        E = (E<<8) | *p;
      }
      p++;
    }
    if(Eneg) {
      /* 2's complement: ... and add 1 (and make negative of course) */
      E = -(E + 1);
    }

    lenN = len - lenE;
    if(lenN > 8)
    {
      /* we can't handle integers > 64 bits */
      DISSECTOR_ASSERT_NOT_REACHED();
    }
    for (i=0; i<lenN; i++) {
      N = (N<<8) | *p;
      p++;
    }
    val = (double) S * N * pow(2, F) * pow(B, E);
#ifdef DEBUG
    ws_debug_printf("S = %d, N = %lu, F = %u, B = %u, E = %d -> %f\n", S, N, F, B, E, val);
#endif
  } else if (octet & 0x40) {  /* SpecialRealValue */
    switch (octet & 0x3F) {
      case 0x00: val = HUGE_VAL; break;
      case 0x01: val = -HUGE_VAL; break;
    }
  } else {  /* decimal encoding */
    buf = g_strndup(p, len);
    val = g_ascii_strtod(buf, NULL);
    g_free(buf);
  }

  return val;
}