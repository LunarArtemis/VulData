static void FVToggleCharSelected(FontView *fv,int enc) {
    int i, j;

    if ( fv->v==NULL || fv->colcnt==0 )	/* Can happen in scripts */
return;

    i = enc / fv->colcnt;
    j = enc - i*fv->colcnt;
    i -= fv->rowoff;
 /* Normally we should be checking against fv->rowcnt (rather than <=rowcnt) */
 /*  but every now and then the WM forces us to use a window size which doesn't */
 /*  fit our expectations (maximized view) and we must be prepared for half */
 /*  lines */
    if ( i>=0 && i<=fv->rowcnt )
	FVDrawGlyph(fv->v,fv,enc,true);
}