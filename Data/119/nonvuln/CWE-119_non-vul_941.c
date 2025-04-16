static void cblistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *sf = fv->b.sf;
    int i, anyligs=0, anykerns=0, gid;
    PST *pst;

    if ( sf->kerns ) anykerns=true;
    for ( i=0; i<fv->b.map->enccount; ++i ) if ( (gid = fv->b.map->map[i])!=-1 && sf->glyphs[gid]!=NULL ) {
	for ( pst=sf->glyphs[gid]->possub; pst!=NULL; pst=pst->next ) {
	    if ( pst->type==pst_ligature ) {
		anyligs = true;
		if ( anykerns )
    break;
	    }
	}
	if ( sf->glyphs[gid]->kerns!=NULL ) {
	    anykerns = true;
	    if ( anyligs )
    break;
	}
    }

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_Ligatures:
	    mi->ti.disabled = !anyligs;
	  break;
	  case MID_KernPairs:
	    mi->ti.disabled = !anykerns;
	  break;
	  case MID_AnchorPairs:
	    mi->ti.disabled = sf->anchor==NULL;
	  break;
	}
    }
}