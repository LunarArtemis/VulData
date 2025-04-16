static void FontView_ReformatAll(SplineFont *sf) {
    BDFFont *new, *old, *bdf;
    FontView *fv;
    MetricsView *mvs;
    extern int use_freetype_to_rasterize_fv;

    if ( sf->fv==NULL || ((FontView *) (sf->fv))->v==NULL || ((FontView *) (sf->fv))->colcnt==0 )			/* Can happen in scripts */
return;

    for ( fv=(FontView *) (sf->fv); fv!=NULL; fv=(FontView *) (fv->b.nextsame) ) {
	GDrawSetCursor(fv->v,ct_watch);
	old = fv->filled;
				/* In CID fonts fv->b.sf may not be same as sf */
	new = SplineFontPieceMeal(fv->b.sf,fv->b.active_layer,fv->filled->pixelsize,72,
		(fv->antialias?pf_antialias:0)|(fv->bbsized?pf_bbsized:0)|
		    (use_freetype_to_rasterize_fv && !sf->strokedfont && !sf->multilayer?pf_ft_nohints:0),
		NULL);
	fv->filled = new;
	if ( fv->show==old )
	    fv->show = new;
	else {
	    for ( bdf=sf->bitmaps; bdf != NULL &&
		( bdf->pixelsize != fv->show->pixelsize || BDFDepth( bdf ) != BDFDepth( fv->show )); bdf=bdf->next );
	    if ( bdf != NULL ) fv->show = bdf;
	    else fv->show = new;
	}
	BDFFontFree(old);
	fv->rowltot = (fv->b.map->enccount+fv->colcnt-1)/fv->colcnt;
	GScrollBarSetBounds(fv->vsb,0,fv->rowltot,fv->rowcnt);
	if ( fv->rowoff>fv->rowltot-fv->rowcnt ) {
	    fv->rowoff = fv->rowltot-fv->rowcnt;
	    if ( fv->rowoff<0 ) fv->rowoff =0;
	    GScrollBarSetPos(fv->vsb,fv->rowoff);
	}
	GDrawRequestExpose(fv->v,NULL,false);
	GDrawSetCursor(fv->v,ct_pointer);
    }
    for ( mvs=sf->metrics; mvs!=NULL; mvs=mvs->next ) if ( mvs->bdf==NULL ) {
	BDFFontFree(mvs->show);
	mvs->show = SplineFontPieceMeal(sf,mvs->layer,mvs->ptsize,mvs->dpi,
		mvs->antialias?(pf_antialias|pf_ft_recontext):pf_ft_recontext,NULL);
	GDrawRequestExpose(mvs->gw,NULL,false);
    }
}