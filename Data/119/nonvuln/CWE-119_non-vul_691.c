static void FVMenuShowSubFont(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *new = mi->ti.userdata;
    FVShowSubFont(fv,new);
}