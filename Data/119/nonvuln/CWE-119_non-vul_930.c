static void FVMenuCondense(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    CondenseExtendDlg(fv,NULL);
}