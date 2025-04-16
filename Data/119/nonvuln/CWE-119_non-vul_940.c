static void FVMenuRemoveFontFromCID(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    char *buts[3];
    SplineFont *cidmaster = fv->b.cidmaster, *sf = fv->b.sf, *replace;
    int i;
    MetricsView *mv, *mnext;
    FontView *fvs;

    if ( cidmaster==NULL || cidmaster->subfontcnt<=1 )	/* Can't remove last font */
return;
    buts[0] = _("_Remove"); buts[1] = _("_Cancel"); buts[2] = NULL;
    if ( gwwv_ask(_("_Remove Font"),(const char **) buts,0,1,_("Are you sure you wish to remove sub-font %1$.40s from the CID font %2$.40s"),
	    sf->fontname,cidmaster->fontname)==1 )
return;

    for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
	CharView *cv, *next;
	for ( cv = (CharView *) (sf->glyphs[i]->views); cv!=NULL; cv = next ) {
	    next = (CharView *) (cv->b.next);
	    GDrawDestroyWindow(cv->gw);
	}
    }
    GDrawProcessPendingEvents(NULL);
    for ( mv=fv->b.sf->metrics; mv!=NULL; mv = mnext ) {
	mnext = mv->next;
	GDrawDestroyWindow(mv->gw);
    }
    GDrawSync(NULL);
    GDrawProcessPendingEvents(NULL);
    /* Just in case... */
    GDrawSync(NULL);
    GDrawProcessPendingEvents(NULL);

    for ( i=0; i<cidmaster->subfontcnt; ++i )
	if ( cidmaster->subfonts[i]==sf )
    break;
    replace = i==0?cidmaster->subfonts[1]:cidmaster->subfonts[i-1];
    while ( i<cidmaster->subfontcnt-1 ) {
	cidmaster->subfonts[i] = cidmaster->subfonts[i+1];
	++i;
    }
    --cidmaster->subfontcnt;

    for ( fvs=(FontView *) (fv->b.sf->fv); fvs!=NULL; fvs=(FontView *) (fvs->b.nextsame) ) {
	if ( fvs->b.sf==sf )
	    CIDSetEncMap((FontViewBase *) fvs,replace);
    }
    FontViewReformatAll(fv->b.sf);
    SplineFontFree(sf);
}