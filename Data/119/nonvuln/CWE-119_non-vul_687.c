static void FVMenuKernPairs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SFKernClassTempDecompose(fv->b.sf,false);
    SFShowKernPairs(fv->b.sf,NULL,NULL,fv->b.active_layer);
    SFKernCleanup(fv->b.sf,false);
}