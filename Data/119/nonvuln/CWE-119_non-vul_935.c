static void lylistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *sf = fv->b.sf;
    int ly;
    GMenuItem *sub;

    sub = calloc(sf->layer_cnt+1,sizeof(GMenuItem));
    for ( ly=ly_fore; ly<sf->layer_cnt; ++ly ) {
	sub[ly-1].ti.text = utf82u_copy(sf->layers[ly].name);
	sub[ly-1].ti.checkable = true;
	sub[ly-1].ti.checked = ly == fv->b.active_layer;
	sub[ly-1].invoke = FVMenuChangeLayer;
	sub[ly-1].mid = ly;
	sub[ly-1].ti.fg = sub[ly-1].ti.bg = COLOR_DEFAULT;
    }
    GMenuItemArrayFree(mi->sub);
    mi->sub = sub;
}