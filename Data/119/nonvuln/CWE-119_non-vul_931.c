static void FVMenuBaseVert(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *sf = fv->b.cidmaster == NULL ? fv->b.sf : fv->b.cidmaster;
    sf->vert_base = SFBaselines(sf,sf->vert_base,true);
    SFBaseSort(sf);
}