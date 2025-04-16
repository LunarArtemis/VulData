static void FVMenuOpenBitmap(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i;
    SplineChar *sc;

    if ( fv->b.cidmaster==NULL ? (fv->b.sf->bitmaps==NULL) : (fv->b.cidmaster->bitmaps==NULL) )
return;
    if ( fv->b.container!=NULL && fv->b.container->funcs->is_modal )
return;
    if ( !FVSelCount(fv))
return;
    for ( i=0; i<fv->b.map->enccount; ++i )
	if ( fv->b.selected[i] ) {
	    sc = FVMakeChar(fv,i);
	    if ( sc!=NULL )
		BitmapViewCreatePick(i,fv);
	}
}