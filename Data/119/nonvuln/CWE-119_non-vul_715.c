static void FontView_Free(FontView *fv) {
    int i;
    FontView *prev;
    FontView *fvs;

    if ( fv->b.sf == NULL )	/* Happens when usurping a font to put it into an MM */
	BDFFontFree(fv->filled);
    else if ( fv->b.nextsame==NULL && fv->b.sf->fv==&fv->b ) {
	EncMapFree(fv->b.map);
	if (fv->b.sf != NULL && fv->b.map == fv->b.sf->map) { fv->b.sf->map = NULL; }
	SplineFontFree(fv->b.cidmaster?fv->b.cidmaster:fv->b.sf);
	BDFFontFree(fv->filled);
    } else {
	EncMapFree(fv->b.map);
	if (fv->b.sf != NULL && fv->b.map == fv->b.sf->map) { fv->b.sf->map = NULL; }
	fv->b.map = NULL;
	for ( fvs=(FontView *) (fv->b.sf->fv), i=0 ; fvs!=NULL; fvs = (FontView *) (fvs->b.nextsame) )
	    if ( fvs->filled==fv->filled ) ++i;
	if ( i==1 )
	    BDFFontFree(fv->filled);
	if ( fv->b.sf->fv==&fv->b ) {
	    if ( fv->b.cidmaster==NULL )
		fv->b.sf->fv = fv->b.nextsame;
	    else {
		fv->b.cidmaster->fv = fv->b.nextsame;
		for ( i=0; i<fv->b.cidmaster->subfontcnt; ++i )
		    fv->b.cidmaster->subfonts[i]->fv = fv->b.nextsame;
	    }
	} else {
	    for ( prev = (FontView *) (fv->b.sf->fv); prev->b.nextsame!=&fv->b; prev=(FontView *) (prev->b.nextsame) );
	    prev->b.nextsame = fv->b.nextsame;
	}
    }
#ifndef _NO_FFSCRIPT
    DictionaryFree(fv->b.fontvars);
    free(fv->b.fontvars);
#endif
    free(fv->b.selected);
    free(fv->fontset);
#ifndef _NO_PYTHON
    PyFF_FreeFV(&fv->b);
#endif
    free(fv);
}