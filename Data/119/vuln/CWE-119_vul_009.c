void frontend(unsigned char *s,
		int ftype,
		char *fname,
		int linenum)
{
	unsigned char *p, *q, c, *begin_end, sep;
	int i, l, str_cnv_p, histo, end_len;
	char prefix_sav[4];
	int latin_sav = 0;		/* have C compiler happy */

	begin_end = NULL;
	end_len = 0;
	histo = 0;
//	state = 0;

	if (ftype == FE_ABC
	 && strncmp((char *) s, "%abc-", 5) == 0) {
		get_vers((char *) s + 5);
		while (*s != '\0'
		    && *s != '\r'
		    && *s != '\n')
			s++;
		if (*s != '\0') {
			s++;
			if (s[-1] == '\r' && *s == '\n')
				s++;
		}
		linenum++;
	}

	/* if unknown encoding, check if latin1 or utf-8 */
	if (ftype == FE_ABC
	 && parse.abc_vers >= ((2 << 16) | (1 << 8))) {	// if ABC version >= 2.1
		latin = 0;				// always UTF-8
	} else {
		for (p = s; *p != '\0'; p++) {
			c = *p;
			if (c == '\\') {
				if (!isdigit(p[1]))
					continue;
				if ((p[1] == '0' || p[1] == '2')
				 && p[2] == '0')	/* accidental */
					continue;
				latin = 1;
				break;
			}
			if (c < 0x80)
				continue;
			if (c >= 0xc2) {
				if ((p[1] & 0xc0) == 0x80) {
					latin = 0;
					break;
				}
			}
			latin = 1;
			break;
		}
	}
	latin_sav = latin;		/* (have gcc happy) */

	/* scan the file */
	skip = 0;
	while (*s != '\0') {

		/* get a line */
		str_cnv_p = 0;
		p = s;
		while (*p != '\0'
		    && *p != '\r'
		    && *p != '\n') {
			if (*p == '\\'
			 || *p == '%'
			 || (latin > 0 && *p >= 0x80))
				str_cnv_p = 1;
			p++;
		}
		l = p - s;
		if (*p != '\0') {
			p++;
			if (p[-1] == '\r' && *p == '\n')	/* (DOS) */
				p++;
		}
		linenum++;

		if (skip) {
			if (l != 0)
				goto ignore;
			skip = 0;
		}
		if (begin_end) {
			if (ftype == FE_FMT) {
				if (strncmp((char *) s, "end", 3) == 0
				 && strncmp((char *) s + 3,
						(char *) begin_end, end_len) == 0) {
					begin_end = NULL;
					goto next_eol;
				}
				if (*s == '%')
					goto ignore;		/* comment */
				goto next;
			}
			if (*s == '%' && strchr(prefix, s[1])) {
				q = s + 2;
				while (*q == ' ' || *q == '\t')
					q++;
				if (strncmp((char *) q, "end", 3) == 0
				 && strncmp((char *) q + 3,
						(char *) begin_end, end_len) == 0) {
					begin_end = NULL;
					goto next_eol;
				}
			}
			if (strncmp("ps", (char *) begin_end, end_len) == 0) {
				if (*s == '%')
					goto ignore;		/* comment */
			} else {
				if (*s == '%' && strchr(prefix, s[1])) {
					s += 2;
					l -= 2;
				}
			}
			goto next;
		}

		while (l > 0 && isspace(s[l - 1]))
			l--;

		if (l == 0) {			/* empty line */
			if (ftype == FE_FMT)
				goto next_eol;
			switch (state) {
			default:
				goto ignore;
			case 1:
				fprintf(stderr,
					"Line %d: Empty line in tune header - K:C added\n",
					linenum);
				txt_add((unsigned char *) "K:C", 3);
				txt_add_eos(fname, linenum);
				/* fall thru */
			case 2:
				state = 0;
				strcpy(prefix, prefix_sav);
				latin = latin_sav;
				break;
			}
			goto next_eol;
		}
		if (histo) {			/* H: continuation */
			if ((s[1] == ':' && isalpha(*s))
			 || (*s == '%' && strchr(prefix, s[1]))) {
				histo = 0;
			} else {
				if (*s != '+' || s[1] != ':')
					txt_add((unsigned char *) "+:", 2);
				goto next;
			}
		}

		/* special case 'space* "%" ' */
		if (*s == ' ' || *s == '\t') {
			q = s;
			do {
				q++;
			} while (*q == ' ' || *q == '\t');
			if (*q == '%')
				goto ignore;
		}

		if (ftype == FE_PS) {
			if (*s == '%')
				goto ignore;
			goto next;
		}

		/* treat the pseudo-comments */
		if (ftype == FE_FMT) {
			if (*s == '%')
				goto ignore;
			goto pscom;
		}
		if (*s == 'I' && s[1] == ':') {
			s += 2;
			l -= 2;
			while (*s == ' ' || *s == '\t') {
				s++;
				l--;
			}
			txt_add((unsigned char *) "%%", 2);
			goto pcinfo;
		}
		if (*s == '%') {
			if (!strchr(prefix, s[1]))	/* pure comment */
				goto ignore;
			s += 2;
			l -= 2;
			if (strncmp((char *) s, "abc ", 4) == 0) {
				s += 4;
				l -= 4;
				goto info;
			}
			if (strncmp((char *) s, "abcm2ps ", 8) == 0
			 || strncmp((char *) s, "ss-pref ", 8) == 0) {
				s += 8;
				l -= 8;
				while (*s == ' ' || *s == '\t') {
					s++;
					l--;
				}
				for (i = 0; i < sizeof prefix - 1; i++) {
					if (*s == ' ' || *s == '\t'
					 || --l < 0)
						break;
					prefix[i] = *s++;
				}
				if (i == 0)
					prefix[i++] = '%';
				prefix[i] = '\0';
				goto ignore;
			}
			if (strncmp((char *) s, "abc-version ", 12) == 0) {
				get_vers((char *) s + 12);
				goto ignore;
			}
pscom:
			while (*s == ' ' || *s == '\t') {
				s++;
				l--;
			}
			txt_add((unsigned char *) "%%", 2);
			if (strncmp((char *) s, "begin", 5) == 0) {
				q = begin_end = s + 5;
				while (!isspace(*q))
					q++;
				end_len = q - begin_end;
				goto next;
			}
pcinfo:
			if (strncmp((char *) s, "encoding ", 9) == 0
			 || strncmp((char *) s, "abc-charset ", 12) == 0) {
				if (*s == 'e')
					q = s + 9;
				else
					q = s + 12;
				while (*q == ' ' || *q == '\t')
					q++;
				if (strncasecmp((char *) q, "latin", 5) == 0) {
					q += 5;
				} else if (strncasecmp((char *) q, "iso-8859-", 9) == 0) {
					q += 9;
				} else if (strncasecmp((char *) q, "utf-8", 5) == 0
					|| strncasecmp((char *) q, "native", 6) == 0) {
					latin = 0;
					goto next;
				} else if (!isdigit(*q)) {
					goto next;	/* unknown charset */
				}
				switch (*q) {
				case '1':
					if (q[1] == '0')
						latin = 6;
					else
						latin = 1;
					break;
				case '2': latin = 2; break;
				case '3': latin = 3; break;
				case '4': latin = 4; break;
				case '5':
					if (q[-1] != '-')
						latin = 5;
					break;
				case '6':
					if (q[-1] != '-')
						latin = 6;
					break;
/*fixme: iso-8859 5..8 not treated */
				case '9': latin = 5; break;
				}
				goto next;
			}
			if (strncmp((char *) s, "format ", 7) == 0
			  || strncmp((char *) s, "abc-include ", 12) == 0) {
				int skip_sav;

				if (*s == 'f')
					s += 7;
				else
					s += 12;
				while (*s == ' ' || *s == '\t')
					s++;
				q = s;
				while (*q != '\0'
				    && *q != '%'
				    && *q != '\n'
				    && *q != '\r')
					q++;
				while (q[-1] == ' ')
					q--;
				sep = *q;
				*q = '\0';
				skip_sav = skip;
//fixme: pb when different encoding in included file: != behaviour .fmt or .abc...
//				latin_sav = latin;
				offset = 0;
				include_file(s);
//				latin = latin_sav;
				skip = skip_sav;
				*q = sep;
				goto ignore;
			}
			if (strncmp((char *) s, "select", 6) == 0) {
				s += 6;
				if (*s == '\n') {	/* select clear */
					q = s;
				} else if (*s != ' ' && *s != '\t') {
					goto next;
				} else {
					while (*s == ' ' || *s == '\t')
						s++;
					q = s;
					while (*q != '\0'
					    && *q != '%'
					    && *q != '\n'
					    && *q != '\r')
						q++;
					while (q[-1] == ' ' || q[-1] == '\t')
						q--;
					if (strncmp((char *) q - 5, " lock", 5) == 0)
						q -= 5;
				}
				if (selection) {
					free(selection);
					selection = NULL;
				}
				if (q != s) {
					sep = *q;
					*q = '\0';
					selection = (unsigned char *) strdup((char *) s);
					*q = sep;
				}
				offset = 0;
				goto ignore;
			}
			goto next;
		}

		/* treat the information fields */
info:
		if (s[1] == ':' && (isalpha(*s) || *s == '+')) {
			c = *s;
			switch (c) {
			case 'I':		/* treat as a pseudo-comment */
				s += 2;
				l -= 2;
				goto pscom;
			case 'X':
				switch (state) {
				case 1:
					fprintf(stderr,
						"Line %d: X: found in tune header - K:C added\n",
						linenum);
					txt_add((unsigned char *) "K:C", 3);
					txt_add_eos(fname, linenum);
					txt_add_eos(fname, linenum);	/* empty line */
					break;
				case 2:
					txt_add_eos(fname, linenum);	/* no empty line - minor error */
					break;
				}
				if (selection) {
					skip = !tune_select(s);
					if (skip)
						goto ignore;
				}
				state = 1;
				strcpy(prefix_sav, prefix);
				latin_sav = latin;
				break;
			case 'U':
				break;
			case 'H':
				histo = 1;
				break;
			default:
				if (state == 0			/* if global */
				 && strchr("dKPQsVWw", *s) != NULL)
					goto ignore;
				if (*s == 'K')
					state = 2;
				break;
			}
			txt_add(s, 2);
			s += 2;
			l -= 2;
			while (*s == ' ' || *s == '\t') {
				s++;
				l--;
			}
			str_cnv_p = 1;
			goto next;
		}

		/* treat the music lines */
		if (state == 0)				/* if not in tune */
			goto ignore;
next:
		if (str_cnv_p)
			txt_add_cnv(s, l, !begin_end);
		else
			txt_add(s, l);
		if (begin_end)
			txt_add((unsigned char *) "\n", 1);
		else
next_eol:
			txt_add_eos(fname, linenum);
ignore:
		s = p;
	}
	if (begin_end)
		fprintf(stderr,
			"Line %d: No %%%%end after %%%%begin\n",
			linenum);
	if (ftype == FE_FMT)
		return;
	if (state == 1)
		fprintf(stderr,
			"Line %d: Unexpected EOF in header definition\n",
			linenum);
	abc_eof();
}