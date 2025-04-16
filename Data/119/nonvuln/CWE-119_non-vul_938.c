static void FVEncodingMenuBuild(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( mi->sub!=NULL ) {
	GMenuItemArrayFree(mi->sub);
	mi->sub = NULL;
    }
    mi->sub = GetEncodingMenu(FVMenuReencode,fv->b.map->enc);
}