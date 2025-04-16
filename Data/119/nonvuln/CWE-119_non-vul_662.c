static void FVMenuSelectByScript(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *e) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    FVSelectByScript(fv,SelMergeType(e));
}