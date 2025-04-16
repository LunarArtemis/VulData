static void GTextFieldSelectWord(GTextField *gt,int mid, int16 *start, int16 *end) {
    unichar_t *text;
    unichar_t ch = gt->text[mid];

    text = gt->text;
    ch = text[mid];

    if ( ch=='\0' )
	*start = *end = mid;
    else if ( (ch<0x10000 && isspace(ch)) ) {
	int i;
	for ( i=mid; text[i]<0x10000 && isspace(text[i]); ++i );
	*end = i;
	for ( i=mid-1; i>=0 && text[i]<0x10000 && isspace(text[i]) ; --i );
	*start = i+1;
    } else if ( (ch<0x10000 && isalnum(ch)) || ch=='_' ) {
	int i;
	for ( i=mid; (text[i]<0x10000 && isalnum(text[i])) || text[i]=='_' ; ++i );
	*end = i;
	for ( i=mid-1; i>=0 && ((text[i]<0x10000 && isalnum(text[i])) || text[i]=='_') ; --i );
	*start = i+1;
    } else {
	int i;
	for ( i=mid; !(text[i]<0x10000 && isalnum(text[i])) && text[i]!='_' && text[i]!='\0' ; ++i );
	*end = i;
	for ( i=mid-1; i>=0 && !(text[i]<0x10000 && isalnum(text[i])) && text[i]!='_' ; --i );
	*start = i+1;
    }
}