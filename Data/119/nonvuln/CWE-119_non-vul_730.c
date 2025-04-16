static void FVMenuDetachGlyphs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVDetachGlyphs((FontViewBase *) fv);
}