static void FVMenuRemoveUndoes(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SFRemoveUndoes(fv->b.sf,fv->b.selected,fv->b.map);
}