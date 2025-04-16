void FVRegenChar(FontView *fv,SplineChar *sc) {
    struct splinecharlist *dlist;
    MetricsView *mv;

    if ( fv->v==NULL )			/* Can happen in scripts */
return;

    if ( sc->orig_pos<fv->filled->glyphcnt ) {
	BDFCharFree(fv->filled->glyphs[sc->orig_pos]);
	fv->filled->glyphs[sc->orig_pos] = NULL;
    }
    /* FVRefreshChar does NOT do this for us */
    for ( mv=fv->b.sf->metrics; mv!=NULL; mv=mv->next )
	MVRegenChar(mv,sc);

    FVRefreshChar(fv,sc->orig_pos);
#if HANYANG
    if ( sc->compositionunit && fv->b.sf->rules!=NULL )
	Disp_RefreshChar(fv->b.sf,sc);
#endif

    for ( dlist=sc->dependents; dlist!=NULL; dlist=dlist->next )
	FVRegenChar(fv,dlist->sc);
}