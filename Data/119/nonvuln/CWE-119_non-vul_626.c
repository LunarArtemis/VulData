static void FVMenuSelectHintingNeeded(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *e) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i, gid, doit;
    EncMap *map = fv->b.map;
    SplineFont *sf = fv->b.sf;
    int order2 = sf->layers[fv->b.active_layer].order2;
    int merge = SelMergeType(e);

    for ( i=0; i< map->enccount; ++i ) {
	doit = ( (gid=map->map[i])!=-1 && sf->glyphs[gid]!=NULL &&
		((!order2 && sf->glyphs[gid]->changedsincelasthinted ) ||
		 ( order2 && sf->glyphs[gid]->layers[fv->b.active_layer].splines!=NULL &&
		     sf->glyphs[gid]->ttf_instrs_len<=0 ) ||
		 ( order2 && sf->glyphs[gid]->instructions_out_of_date )) );
	fv->b.selected[i] = mergefunc[ merge + (fv->b.selected[i]?2:0) + doit ];
    }
    GDrawRequestExpose(fv->v,NULL,false);
}