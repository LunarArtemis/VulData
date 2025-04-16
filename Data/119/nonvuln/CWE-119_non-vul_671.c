static void FVMenuKPCloseup(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i;

    for ( i=0; i<fv->b.map->enccount; ++i )
	if ( fv->b.selected[i] )
    break;
    KernPairD(fv->b.sf,i==fv->b.map->enccount?NULL:
		    fv->b.map->map[i]==-1?NULL:
		    fv->b.sf->glyphs[fv->b.map->map[i]],NULL,fv->b.active_layer,
		    false);
}