static void FVMenuClearBackground(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVClearBackground( (FontViewBase *) fv );
}