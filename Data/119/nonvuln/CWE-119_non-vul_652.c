static void FVMenuReencode(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    Encoding *enc = NULL;
    SplineChar *sc;

    sc = FVFindACharInDisplay(fv);
    enc = FindOrMakeEncoding(mi->ti.userdata);
    if ( enc==NULL ) {
	IError("Known encoding could not be found");
return;
    }
    FVReencode((FontViewBase *) fv,enc);
    if ( sc!=NULL ) {
	int enc = fv->b.map->backmap[sc->orig_pos];
	if ( enc!=-1 )
	    FVScrollToChar(fv,enc);
    }
}