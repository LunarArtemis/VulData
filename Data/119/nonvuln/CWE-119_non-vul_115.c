static void txt_add_eos(char *fname, int linenum)
{
	static unsigned char eos = '\0';

	/* special case for continuation lines in ABC version 2.0 */
	if (parse.abc_vers == (2 << 16)
	 && offset > 0
	 && dst[offset - 1] == '\\') {
		offset--;
		return;
	}
	txt_add(&eos, 1);
	abc_parse((char *) dst, fname, linenum);
	offset = 0;
}