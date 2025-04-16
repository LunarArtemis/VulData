static int tune_select(unsigned char *s)
{
	regex_t r;
	unsigned char *p, *sel;
	int ret;

	/* if there is a list of tune indexes,
	 * check the tune index */
	sel = selection;
	if (isdigit(*sel)) {
		int tune_number, cur_sel, end_sel, n;

		/* get the tune number ('s' points to X:) */
		tune_number = strtod((char *) s + 2, 0);

		/* search it in the number list */
		for (;;) {
			if (sscanf((char *) sel, "%d%n", &cur_sel, &n) != 1)
				break;
			sel += n;
			if (*sel == '-') {
				sel++;
				if (sscanf((char *) sel, "%d%n", &end_sel, &n) != 1)
					end_sel = ~0u >> 1;
				else
					sel += n;
			} else {
				end_sel = cur_sel;
			}
			if (tune_number >= cur_sel && tune_number <= end_sel)
				return 1;
			if (*sel != ',')
				break;
			sel++;
		}
		if (*sel == '\0')
			return 0;
	}

	for (p = s + 2; ; p++) {
		switch (*p) {
		case '\0':
			return 0;
		default:
			continue;
		case '\n':
		case '\r':
			break;
		}
		if (p[1] != 'K' || p[2] != ':')
			continue;
		p += 3;
		while (*p != '\n' && *p != '\r' && *p != '\0')
			p++;
		if (*p != '\0')
			p++;		/* keep the EOL for RE with '\s' */
		break;
	}

	ret = p - s;
	if (ret >= TEX_BUF_SZ - 1) {
		fprintf(stderr, "Tune header too big for %%%%select\n");
		return 0;
	}
	memcpy(tex_buf, s, ret);
	tex_buf[ret] = '\0';

	ret = regcomp(&r, (char *) sel,
				REG_EXTENDED | REG_NEWLINE | REG_NOSUB);
	if (ret)
		return 0;
	ret = regexec(&r, tex_buf, 0, NULL, 0);
	regfree(&r);
	return !ret;
}