static void FVMenuSameGlyphAs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVSameGlyphAs((FontViewBase *) fv);
    GDrawRequestExpose(fv->v,NULL,false);
}