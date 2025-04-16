static void FVMenuBaseHoriz(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *sf = fv->b.cidmaster == NULL ? fv->b.sf : fv->b.cidmaster;
    sf->horiz_base = SFBaselines(sf,sf->horiz_base,false);
    SFBaseSort(sf);
}