static void FVMenuCIDFontInfo(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *cidmaster = fv->b.cidmaster;

    if ( cidmaster==NULL )
return;
    FontInfo(cidmaster,fv->b.active_layer,-1,false);
}