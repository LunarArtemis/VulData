static void FVMenuBuildDuplicate(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVBuildDuplicate( (FontViewBase *) GDrawGetUserData(gw));
}