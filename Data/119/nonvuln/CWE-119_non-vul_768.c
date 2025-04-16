static void FontViewRefreshAll(SplineFont *sf) {
    FontView *fv;
    for ( fv = (FontView *) (sf->fv); fv!=NULL; fv = (FontView *) (fv->b.nextsame) )
	if ( fv->v!=NULL )
	    GDrawRequestExpose(fv->v,NULL,false);
}