static void FVMenuOpenOutline(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i;
    SplineChar *sc;

    if ( !FVSelCount(fv))
return;
    if ( fv->b.container!=NULL && fv->b.container->funcs->is_modal )
return;

    for ( i=0; i<fv->b.map->enccount; ++i )
	if ( fv->b.selected[i] ) {
	    sc = FVMakeChar(fv,i);
	    CharViewCreate(sc,fv,i);
	}
}