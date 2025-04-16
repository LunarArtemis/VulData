static void FVMenuImport(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int empty = fv->b.sf->onlybitmaps && fv->b.sf->bitmaps==NULL;
    BDFFont *bdf;
    FVImport(fv);
    if ( empty && fv->b.sf->bitmaps!=NULL ) {
	for ( bdf= fv->b.sf->bitmaps; bdf->next!=NULL; bdf = bdf->next );
	FVChangeDisplayBitmap((FontViewBase *) fv,bdf);
    }
}