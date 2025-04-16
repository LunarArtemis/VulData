static void FVShowSubFont(FontView *fv,SplineFont *new) {
    MetricsView *mv, *mvnext;
    BDFFont *newbdf;
    int wascompact = fv->b.normal!=NULL;
    extern int use_freetype_to_rasterize_fv;

    for ( mv=fv->b.sf->metrics; mv!=NULL; mv = mvnext ) {
	/* Don't bother trying to fix up metrics views, just not worth it */
	mvnext = mv->next;
	GDrawDestroyWindow(mv->gw);
    }
    if ( wascompact ) {
	EncMapFree(fv->b.map);
	if (fv->b.map == fv->b.sf->map) { fv->b.sf->map = fv->b.normal; }
	fv->b.map = fv->b.normal;
	fv->b.normal = NULL;
	fv->b.selected = realloc(fv->b.selected,fv->b.map->enccount);
	memset(fv->b.selected,0,fv->b.map->enccount);
    }
    CIDSetEncMap((FontViewBase *) fv,new);
    if ( wascompact ) {
	fv->b.normal = EncMapCopy(fv->b.map);
	CompactEncMap(fv->b.map,fv->b.sf);
	FontViewReformatOne(&fv->b);
	FVSetTitle(&fv->b);
    }
    newbdf = SplineFontPieceMeal(fv->b.sf,fv->b.active_layer,fv->filled->pixelsize,72,
	    (fv->antialias?pf_antialias:0)|(fv->bbsized?pf_bbsized:0)|
		(use_freetype_to_rasterize_fv && !fv->b.sf->strokedfont && !fv->b.sf->multilayer?pf_ft_nohints:0),
	    NULL);
    BDFFontFree(fv->filled);
    if ( fv->filled == fv->show )
	fv->show = newbdf;
    fv->filled = newbdf;
    GDrawRequestExpose(fv->v,NULL,true);
}