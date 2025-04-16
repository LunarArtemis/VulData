static void FVMenuMergeKern(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    MergeKernInfo(fv->b.sf,fv->b.map);
}