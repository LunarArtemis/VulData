static void FVMenuWireframe(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    ShadowDlg(fv,NULL,NULL,true);
}