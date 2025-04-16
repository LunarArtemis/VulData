static int FontViewWinInfo(FontView *fv, int *cc, int *rc) {
    if ( fv==NULL || fv->colcnt==0 || fv->rowcnt==0 ) {
	*cc = 16; *rc = 4;
return( -1 );
    }

    *cc = fv->colcnt;
    *rc = fv->rowcnt;

return( fv->rowoff*fv->colcnt );
}