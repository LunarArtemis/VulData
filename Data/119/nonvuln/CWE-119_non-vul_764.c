void _FVCloseWindows(FontView *fv) {
    int i, j;
    BDFFont *bdf;
    MetricsView *mv, *mnext;
    SplineFont *sf = fv->b.cidmaster?fv->b.cidmaster:fv->b.sf->mm!=NULL?fv->b.sf->mm->normal : fv->b.sf;

    PrintWindowClose();
    if ( fv->b.nextsame==NULL && fv->b.sf->fv==&fv->b && fv->b.sf->kcld!=NULL )
	KCLD_End(fv->b.sf->kcld);
    if ( fv->b.nextsame==NULL && fv->b.sf->fv==&fv->b && fv->b.sf->vkcld!=NULL )
	KCLD_End(fv->b.sf->vkcld);

    for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
	CharView *cv, *next;
	for ( cv = (CharView *) (sf->glyphs[i]->views); cv!=NULL; cv = next ) {
	    next = (CharView *) (cv->b.next);
	    GDrawDestroyWindow(cv->gw);
	}
	if ( sf->glyphs[i]->charinfo )
	    CharInfoDestroy(sf->glyphs[i]->charinfo);
    }
    if ( sf->mm!=NULL ) {
	MMSet *mm = sf->mm;
	for ( j=0; j<mm->instance_count; ++j ) {
	    SplineFont *sf = mm->instances[j];
	    for ( i=0; i<sf->glyphcnt; ++i ) if ( sf->glyphs[i]!=NULL ) {
		CharView *cv, *next;
		for ( cv = (CharView *) (sf->glyphs[i]->views); cv!=NULL; cv = next ) {
		    next = (CharView *) (cv->b.next);
		    GDrawDestroyWindow(cv->gw);
		}
		if ( sf->glyphs[i]->charinfo )
		    CharInfoDestroy(sf->glyphs[i]->charinfo);
	    }
	    for ( mv=sf->metrics; mv!=NULL; mv = mnext ) {
		mnext = mv->next;
		GDrawDestroyWindow(mv->gw);
	    }
	}
    } else if ( sf->subfontcnt!=0 ) {
	for ( j=0; j<sf->subfontcnt; ++j ) {
	    for ( i=0; i<sf->subfonts[j]->glyphcnt; ++i ) if ( sf->subfonts[j]->glyphs[i]!=NULL ) {
		CharView *cv, *next;
		for ( cv = (CharView *) (sf->subfonts[j]->glyphs[i]->views); cv!=NULL; cv = next ) {
		    next = (CharView *) (cv->b.next);
		    GDrawDestroyWindow(cv->gw);
		if ( sf->subfonts[j]->glyphs[i]->charinfo )
		    CharInfoDestroy(sf->subfonts[j]->glyphs[i]->charinfo);
		}
	    }
	    for ( mv=sf->subfonts[j]->metrics; mv!=NULL; mv = mnext ) {
		mnext = mv->next;
		GDrawDestroyWindow(mv->gw);
	    }
	}
    } else {
	for ( mv=sf->metrics; mv!=NULL; mv = mnext ) {
	    mnext = mv->next;
	    GDrawDestroyWindow(mv->gw);
	}
    }
    for ( bdf = sf->bitmaps; bdf!=NULL; bdf=bdf->next ) {
	for ( i=0; i<bdf->glyphcnt; ++i ) if ( bdf->glyphs[i]!=NULL ) {
	    BitmapView *bv, *next;
	    for ( bv = bdf->glyphs[i]->views; bv!=NULL; bv = next ) {
		next = bv->next;
		GDrawDestroyWindow(bv->gw);
	    }
	}
    }
    if ( fv->b.sf->fontinfo!=NULL )
	FontInfoDestroy(fv->b.sf);
    if ( fv->b.sf->valwin!=NULL )
	ValidationDestroy(fv->b.sf);
    SVDetachFV(fv);
}