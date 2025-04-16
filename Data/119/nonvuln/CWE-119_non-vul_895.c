static void FVSelectColor(FontView *fv, uint32 col, int merge) {
    int i, doit;
    uint32 sccol;
    SplineChar **glyphs = fv->b.sf->glyphs;

    for ( i=0; i<fv->b.map->enccount; ++i ) {
	int gid = fv->b.map->map[i];
	sccol =  ( gid==-1 || glyphs[gid]==NULL ) ? COLOR_DEFAULT : glyphs[gid]->color;
	doit = sccol==col;
	fv->b.selected[i] = mergefunc[ merge + (fv->b.selected[i]?2:0) + doit ];
    }
    GDrawRequestExpose(fv->v,NULL,false);
}