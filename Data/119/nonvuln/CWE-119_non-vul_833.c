static void FVMenuClearSpecialData(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVClearSpecialData((FontViewBase *) fv);
}