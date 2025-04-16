static void utf82u_annot_strncat(unichar_t *to, const char *from, int len) {
    register unichar_t ch;

    to += u_strlen(to);
    while ( (ch = utf8_ildb(&from)) != '\0' && --len>=0 ) {
	if ( ch=='\t' ) {
	    *(to++) = ' ';
	    ch = ' ';
	}
	*(to++) = ch;
    }
    *to = 0;
}