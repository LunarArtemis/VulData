static int split(char *buf, char **ptr)
{
	int i;

	for (i = 0; i < 4; i++) {
		buf = skip_word(buf);
		if (!*buf)
			return i;

		*buf = 0;

		buf = skip_space(buf + 1);
		if (!*buf)
			return i;

		ptr[i] = buf;
	}

	buf = skip_word(buf);
	//if (*buf == '\n')
		*buf = 0;
	//else if (*buf)
	//	return -1;

	return i;
}