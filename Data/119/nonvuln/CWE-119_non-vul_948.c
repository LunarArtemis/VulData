unichar_t *_GGadgetFileToUString(char *filename,int max) {
    FILE *file;
    int ch, ch2, ch3;
    int format=0;
    unichar_t *space, *upt, *end;

    file = fopen( filename,"r" );
    if ( file==NULL )
return( NULL );
    ch = getc(file); ch2 = getc(file); ch3 = getc(file);
    ungetc(ch3,file);
    if ( ch==0xfe && ch2==0xff )
	format = 1;		/* normal ucs2 */
    else if ( ch==0xff && ch2==0xfe )
	format = 2;		/* byte-swapped ucs2 */
    else if ( ch==0xef && ch2==0xbb && ch3==0xbf ) {
	format = 3;		/* utf8 */
	getc(file);
    } else {
	getc(file);		/* rewind probably undoes the ungetc, but let's not depend on it */
	rewind(file);
    }
    space = upt = malloc((max+1)*sizeof(unichar_t));
    end = space+max;
    if ( format==3 ) {		/* utf8 */
	while ( upt<end ) {
	    ch=getc(file);
	    if ( ch==EOF )
	break;
	    if ( ch<0x80 )
		*upt++ = ch;
	    else if ( ch<0xe0 ) {
		ch2 = getc(file);
		*upt++ = ((ch&0x1f)<<6)|(ch2&0x3f);
	    } else if ( ch<0xf0 ) {
		ch2 = getc(file); ch3 = getc(file);
		*upt++ = ((ch&0xf)<<12)|((ch2&0x3f)<<6)|(ch3&0x3f);
	    } else {
		int ch4, w;
		ch2 = getc(file); ch3 = getc(file); ch4=getc(file);
		w = ( ((ch&7)<<2) | ((ch2&0x30)>>4) ) -1;
		*upt++ = 0xd800 | (w<<6) | ((ch2&0xf)<<2) | ((ch3&0x30)>>4);
		if ( upt<end )
		    *upt++ = 0xdc00 | ((ch3&0xf)<<6) | (ch4&0x3f);
	    }
	}
    } else if ( format!=0 ) {
	while ( upt<end ) {
	    ch = getc(file); ch2 = getc(file);
	    if ( ch2==EOF )
	break;
	    if ( format==1 )
		*upt ++ = (ch<<8)|ch2;
	    else
		*upt ++ = (ch2<<8)|ch;
	}
    } else {
	char buffer[400];
	while ( fgets(buffer,sizeof(buffer),file)!=NULL ) {
	    def2u_strncpy(upt,buffer,end-upt);
	    upt += u_strlen(upt);
	}
    }
    *upt = '\0';
    fclose(file);
return( space );
}