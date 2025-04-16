static void FVForceEncodingMenuBuild(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( mi->sub!=NULL ) {
	GMenuItemArrayFree(mi->sub);
	mi->sub = NULL;
    }
    mi->sub = GetEncodingMenu(FVMenuForceEncode,fv->b.map->enc);
}