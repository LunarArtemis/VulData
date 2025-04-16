static void FVMenuChangeChar(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    _FVMenuChangeChar(fv,mi->mid);
}