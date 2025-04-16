static void balistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
        if ( mi->mid==MID_BuildAccent || mi->mid==MID_BuildComposite ) {
	    int anybuildable = false;
	    int onlyaccents = mi->mid==MID_BuildAccent;
	    int i, gid;
	    for ( i=0; i<fv->b.map->enccount; ++i ) if ( fv->b.selected[i] ) {
		SplineChar *sc=NULL, dummy;
		if ( (gid=fv->b.map->map[i])!=-1 )
		    sc = fv->b.sf->glyphs[gid];
		if ( sc==NULL )
		    sc = SCBuildDummy(&dummy,fv->b.sf,fv->b.map,i);
		if ( SFIsSomethingBuildable(fv->b.sf,sc,fv->b.active_layer,onlyaccents)) {
		    anybuildable = true;
	    break;
		}
	    }
	    mi->ti.disabled = !anybuildable;
        } else if ( mi->mid==MID_BuildDuplicates ) {
	    int anybuildable = false;
	    int i, gid;
	    for ( i=0; i<fv->b.map->enccount; ++i ) if ( fv->b.selected[i] ) {
		SplineChar *sc=NULL, dummy;
		if ( (gid=fv->b.map->map[i])!=-1 )
		    sc = fv->b.sf->glyphs[gid];
		if ( sc==NULL )
		    sc = SCBuildDummy(&dummy,fv->b.sf,fv->b.map,i);
		if ( SFIsDuplicatable(fv->b.sf,sc)) {
		    anybuildable = true;
	    break;
		}
	    }
	    mi->ti.disabled = !anybuildable;
	}
    }
}