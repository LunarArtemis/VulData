static void FVMenuRevert(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontViewBase *fv = (FontViewBase *) GDrawGetUserData(gw);
    FVRevert(fv);
}