static void FVMenuBuildAccent(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVBuildAccent( (FontViewBase *) GDrawGetUserData(gw), true );
}