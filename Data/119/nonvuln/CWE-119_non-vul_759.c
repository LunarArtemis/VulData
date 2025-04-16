static void FVMenuMATHInfo(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SFMathDlg(fv->b.sf,fv->b.active_layer);
}