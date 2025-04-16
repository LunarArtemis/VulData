static void FVMenuFlatten(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *cidmaster = fv->b.cidmaster;

    if ( cidmaster==NULL )
return;
    SFFlatten(&cidmaster);
}