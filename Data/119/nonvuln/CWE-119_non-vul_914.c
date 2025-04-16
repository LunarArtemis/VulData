void FVRefreshChar(FontView *fv,int gid) {
    BDFChar *bdfc;
    int i, j, enc;
    MetricsView *mv;

    /* Can happen in scripts */ /* Can happen if we do an AutoHint when generating a tiny font for freetype context */
    if ( fv->v==NULL || fv->colcnt==0 || fv->b.sf->glyphs[gid]== NULL )
return;
    for ( fv=(FontView *) (fv->b.sf->fv); fv!=NULL; fv = (FontView *) (fv->b.nextsame) ) {
	if( !fv->colcnt )
	    continue;

	for ( mv=fv->b.sf->metrics; mv!=NULL; mv=mv->next )
	    MVRefreshChar(mv,fv->b.sf->glyphs[gid]);
	if ( fv->show==fv->filled )
	    bdfc = BDFPieceMealCheck(fv->show,gid);
	else
	    bdfc = fv->show->glyphs[gid];
	if ( bdfc==NULL )
	    bdfc = BDFPieceMeal(fv->show,gid);
	/* A glyph may be encoded in several places, all need updating */
	for ( enc = 0; enc<fv->b.map->enccount; ++enc ) if ( fv->b.map->map[enc]==gid ) {
	    i = enc / fv->colcnt;
	    j = enc - i*fv->colcnt;
	    i -= fv->rowoff;
	    if ( i>=0 && i<fv->rowcnt )
		FVDrawGlyph(fv->v,fv,enc,true);
	}
    }
}