int 
mbfl_oddlen(mbfl_string *string)
{
	int len, n, m, k;
	unsigned char *p;
	const unsigned char *mbtab;
	const mbfl_encoding *encoding;


	if (string == NULL) {
		return -1;
	}
	encoding = mbfl_no2encoding(string->no_encoding);
	if (encoding == NULL) {
		return -1;
	}

	len = 0;
	if (encoding->flag & MBFL_ENCTYPE_SBCS) {
		return 0;
	} else if (encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
		return len % 2;
	} else if (encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
		return len % 4;
	} else if (encoding->mblen_table != NULL) {
 		mbtab = encoding->mblen_table;
 		n = 0;
		p = string->val;
		k = string->len;
		/* count */
		if (p != NULL) {
			while (n < k) {
				m = mbtab[*p];
				n += m;
				p += m;
			};
		}
		return n-k;
	} else {
		/* how can i do ? */
		return 0;
	}
	/* NOT REACHED */
}