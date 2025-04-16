static void FVMenuCut(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVCopy(&fv->b,ct_fullcopy);
    FVClear(&fv->b);
}