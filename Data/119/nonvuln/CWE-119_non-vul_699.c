static void FVMenuShowAtt(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    ShowAtt(fv->b.sf,fv->b.active_layer);
}