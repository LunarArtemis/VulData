static void FVMenuOpen(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView*) GDrawGetUserData(gw);
    _FVMenuOpen(fv);
}