static int GTextFieldSelBackword(unichar_t *text,int start) {
    unichar_t ch = text[start-1];

    if ( start==0 )
	/* Can't go back */;
    else if ( isalnum(ch) || ch=='_' ) {
	int i;
	for ( i=start-1; i>=0 && ((text[i]<0x10000 && isalnum(text[i])) || text[i]=='_') ; --i );
	start = i+1;
    } else {
	int i;
	for ( i=start-1; i>=0 && !(text[i]<0x10000 && isalnum(text[i])) && text[i]!='_' ; --i );
	start = i+1;
    }
return( start );
}