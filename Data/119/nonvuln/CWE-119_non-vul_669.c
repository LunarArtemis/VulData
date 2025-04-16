static void FVExtraEncSlots(FontView *fv, int encmax) {
    if ( fv->colcnt!=0 ) {		/* Ie. scripting vs. UI */
	fv->rowltot = (encmax+1+fv->colcnt-1)/fv->colcnt;
	GScrollBarSetBounds(fv->vsb,0,fv->rowltot,fv->rowcnt);
    }
}