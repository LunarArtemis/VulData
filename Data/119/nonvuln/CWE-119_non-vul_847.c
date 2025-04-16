static void FVMenuModifyComposition(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    if ( fv->b.sf->rules!=NULL )
	SFModifyComposition(fv->b.sf);
}