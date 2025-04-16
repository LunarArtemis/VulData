static void FVMenuLigatures(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SFShowLigatures(fv->b.sf,NULL);
}