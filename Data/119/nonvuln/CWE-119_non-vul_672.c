static void FVMenuSelectChanged(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *e) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i, gid, doit;
    EncMap *map = fv->b.map;
    SplineFont *sf = fv->b.sf;
    int merge = SelMergeType(e);

    for ( i=0; i< map->enccount; ++i ) {
	doit = ( (gid=map->map[i])!=-1 && sf->glyphs[gid]!=NULL && sf->glyphs[gid]->changed );
	fv->b.selected[i] = mergefunc[ merge + (fv->b.selected[i]?2:0) + doit ];
    }

    GDrawRequestExpose(fv->v,NULL,false);
}