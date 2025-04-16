static void do_Adobe_Pua(unichar_t *buf,int sob,int uni) {
    int i, j;

    for ( i=j=0; j<sob-1 && i<3; ++i ) {
	int ch = adobes_pua_alts[uni-0xf600][i];
	if ( ch==0 )
    break;
	if ( ch>=0xf600 && ch<=0xf7ff && adobes_pua_alts[ch-0xf600]!=0 ) {
	    do_Adobe_Pua(buf+j,sob-j,ch);
	    while ( buf[j]!=0 ) ++j;
	} else
	    buf[j++] = ch;
    }
    buf[j] = 0;
}