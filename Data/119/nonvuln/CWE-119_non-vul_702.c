static void FontViewOpenKids(FontView *fv) {
    int k, i;
    SplineFont *sf = fv->b.sf, *_sf;
#if defined(__Mac)
    int cnt= 0;
#endif

    if ( sf->cidmaster!=NULL )
	sf = sf->cidmaster;

    k=0;
    do {
	_sf = sf->subfontcnt==0 ? sf : sf->subfonts[k];
	for ( i=0; i<_sf->glyphcnt; ++i )
	    if ( _sf->glyphs[i]!=NULL && _sf->glyphs[i]->wasopen ) {
		_sf->glyphs[i]->wasopen = false;
#if defined(__Mac)
		/* If we open a bunch of charviews all at once on the mac, X11*/
		/*  crashes */ /* But opening one seems ok */
		if ( ++cnt==1 )
#endif
		CharViewCreate(_sf->glyphs[i],fv,-1);
	    }
	++k;
    } while ( k<sf->subfontcnt );
}