static void FVMenuCompact(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineChar *sc;

    sc = FVFindACharInDisplay(fv);
    FVCompact((FontViewBase *) fv);
    if ( sc!=NULL ) {
	int enc = fv->b.map->backmap[sc->orig_pos];
	if ( enc!=-1 )
	    FVScrollToChar(fv,enc);
    }
}