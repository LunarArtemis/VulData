static void FVMenuRemoveUnused(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVRemoveUnused((FontViewBase *) fv);
}