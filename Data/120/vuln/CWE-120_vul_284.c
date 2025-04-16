int seq_buf_putmem_hex(struct seq_buf *s, const void *mem,
		       unsigned int len)
{
	unsigned char hex[HEX_CHARS];
	const unsigned char *data = mem;
	unsigned int start_len;
	int i, j;

	WARN_ON(s->size == 0);

	while (len) {
		start_len = min(len, HEX_CHARS - 1);
#ifdef __BIG_ENDIAN
		for (i = 0, j = 0; i < start_len; i++) {
#else
		for (i = start_len-1, j = 0; i >= 0; i--) {
#endif
			hex[j++] = hex_asc_hi(data[i]);
			hex[j++] = hex_asc_lo(data[i]);
		}
		if (WARN_ON_ONCE(j == 0 || j/2 > len))
			break;

		/* j increments twice per loop */
		len -= j / 2;
		hex[j++] = ' ';

		seq_buf_putmem(s, hex, j);
		if (seq_buf_has_overflowed(s))
			return -1;
	}
	return 0;
}