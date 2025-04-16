static void delistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i = FVAnyCharSelected(fv);
    int gid = i<0 ? -1 : fv->b.map->map[i];

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_ShowDependentRefs:
	    mi->ti.disabled = gid<0 || fv->b.sf->glyphs[gid]==NULL ||
		    fv->b.sf->glyphs[gid]->dependents == NULL;
	  break;
	  case MID_ShowDependentSubs:
	    mi->ti.disabled = gid<0 || fv->b.sf->glyphs[gid]==NULL ||
		    !SCUsedBySubs(fv->b.sf->glyphs[gid]);
	  break;
	}
    }
}