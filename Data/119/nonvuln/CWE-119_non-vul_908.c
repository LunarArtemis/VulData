static void FVMenuBuildSyllables(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    if ( fv->b.sf->rules!=NULL )
	SFBuildSyllables(fv->b.sf);
}