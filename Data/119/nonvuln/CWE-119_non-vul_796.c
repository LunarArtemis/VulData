static void FVMenuBuildComposite(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVBuildAccent( (FontViewBase *) GDrawGetUserData(gw), false );
}