static void FVMenuGlyphsSplines(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *e) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i, gid, doit;
    EncMap *map = fv->b.map;
    SplineFont *sf = fv->b.sf;
    int merge = SelMergeType(e);
    int layer = fv->b.active_layer;

    for ( i=0; i< map->enccount; ++i ) {
	doit = ( (gid=map->map[i])!=-1 && sf->glyphs[gid]!=NULL &&
		sf->glyphs[gid]->layers[layer].refs==NULL &&
		sf->glyphs[gid]->layers[layer].splines!=NULL );
	fv->b.selected[i] = mergefunc[ merge + (fv->b.selected[i]?2:0) + doit ];
    }
    GDrawRequestExpose(fv->v,NULL,false);
}