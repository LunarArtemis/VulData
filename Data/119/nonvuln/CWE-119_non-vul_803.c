static void FVMenuChangeXHeight(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    ChangeXHeightDlg(fv,NULL);
}