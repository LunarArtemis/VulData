static void FVMenuAnchorPairs(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SFShowKernPairs(fv->b.sf,NULL,mi->ti.userdata,fv->b.active_layer);
}