static FontView *__FontViewCreate(SplineFont *sf) {
    FontView *fv = calloc(1,sizeof(FontView));
    int i;
    int ps = sf->display_size<0 ? -sf->display_size :
	     sf->display_size==0 ? default_fv_font_size : sf->display_size;

    if ( ps>200 ) ps = 128;

    /* Filename != NULL if we opened an sfd file. Sfd files know whether */
    /*  the font is compact or not and should not depend on a global flag */
    /* If a font is new, then compaction will make it vanish completely */
    if ( sf->fv==NULL && compact_font_on_open && sf->filename==NULL && !sf->new ) {
	sf->compacted = true;
	for ( i=0; i<sf->subfontcnt; ++i )
	    sf->subfonts[i]->compacted = true;
    }
    fv->b.nextsame = sf->fv;
    fv->b.active_layer = sf->display_layer;
    sf->fv = (FontViewBase *) fv;
    if ( sf->mm!=NULL ) {
	sf->mm->normal->fv = (FontViewBase *) fv;
	for ( i = 0; i<sf->mm->instance_count; ++i )
	    sf->mm->instances[i]->fv = (FontViewBase *) fv;
    }
    if ( sf->subfontcnt==0 ) {
	fv->b.sf = sf;
	if ( fv->b.nextsame!=NULL ) {
	    fv->b.map = EncMapCopy(fv->b.nextsame->map);
	    fv->b.normal = fv->b.nextsame->normal==NULL ? NULL : EncMapCopy(fv->b.nextsame->normal);
	} else if ( sf->compacted ) {
	    fv->b.normal = sf->map;
	    fv->b.map = CompactEncMap(EncMapCopy(sf->map),sf);
	    sf->map = fv->b.map;
	} else {
	    fv->b.map = sf->map;
	    fv->b.normal = NULL;
	}
    } else {
	fv->b.cidmaster = sf;
	for ( i=0; i<sf->subfontcnt; ++i )
	    sf->subfonts[i]->fv = (FontViewBase *) fv;
	for ( i=0; i<sf->subfontcnt; ++i )	/* Search for a subfont that contains more than ".notdef" (most significant in .gai fonts) */
	    if ( sf->subfonts[i]->glyphcnt>1 ) {
		fv->b.sf = sf->subfonts[i];
	break;
	    }
	if ( fv->b.sf==NULL )
	    fv->b.sf = sf->subfonts[0];
	sf = fv->b.sf;
	if ( fv->b.nextsame==NULL ) { EncMapFree(sf->map); sf->map = NULL; }
	fv->b.map = EncMap1to1(sf->glyphcnt);
	if ( fv->b.nextsame==NULL ) { sf->map = fv->b.map; }
	if ( sf->compacted ) {
	    fv->b.normal = fv->b.map;
	    fv->b.map = CompactEncMap(EncMapCopy(fv->b.map),sf);
	    if ( fv->b.nextsame==NULL ) { sf->map = fv->b.map; }
	}
    }
    fv->b.selected = calloc((fv->b.map ? fv->b.map->enccount : 0), sizeof(char));
    fv->user_requested_magnify = -1;
    fv->magnify = (ps<=9)? 3 : (ps<20) ? 2 : 1;
    fv->cbw = (ps*fv->magnify)+1;
    fv->cbh = (ps*fv->magnify)+1+fv->lab_height+1;
    fv->antialias = sf->display_antialias;
    fv->bbsized = sf->display_bbsized;
    fv->glyphlabel = default_fv_glyphlabel;

    fv->end_pos = -1;
#ifndef _NO_PYTHON
    PyFF_InitFontHook((FontViewBase *)fv);
#endif

    fv->pid_webfontserver = 0;

return( fv );
}