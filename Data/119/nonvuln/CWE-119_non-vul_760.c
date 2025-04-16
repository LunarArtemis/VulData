static void FVMenuGotoChar(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int merge_with_selection = false;
    int pos = GotoChar(fv->b.sf,fv->b.map,&merge_with_selection);
    if ( fv->b.cidmaster!=NULL && pos!=-1 && !fv->b.map->enc->is_compact ) {
	SplineFont *cidmaster = fv->b.cidmaster;
	int k, hadk= cidmaster->subfontcnt;
	for ( k=0; k<cidmaster->subfontcnt; ++k ) {
	    SplineFont *sf = cidmaster->subfonts[k];
	    if ( pos<sf->glyphcnt && sf->glyphs[pos]!=NULL )
	break;
	    if ( pos<sf->glyphcnt )
		hadk = k;
	}
	if ( k==cidmaster->subfontcnt && pos>=fv->b.sf->glyphcnt )
	    k = hadk;
	if ( k!=cidmaster->subfontcnt && cidmaster->subfonts[k] != fv->b.sf )
	    FVShowSubFont(fv,cidmaster->subfonts[k]);
	if ( pos>=fv->b.sf->glyphcnt )
	    pos = -1;
    }
    if ( !merge_with_selection )
	FVChangeChar(fv,pos);
    else {
	if ( !fv->b.selected[pos] ) {
	    fv->b.selected[pos] = ++fv->sel_index;
	    FVToggleCharSelected(fv,pos);
	}
	fv->end_pos = fv->pressed_pos = pos;
	FVScrollToChar(fv,pos);
	FVShowInfo(fv);
    }
}