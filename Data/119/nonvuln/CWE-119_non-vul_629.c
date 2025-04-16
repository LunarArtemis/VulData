static void aplistbuild(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    GMenuItemArrayFree(mi->sub);
    mi->sub = NULL;

    _aplistbuild(mi,fv->b.sf,FVMenuAnchorPairs);
}