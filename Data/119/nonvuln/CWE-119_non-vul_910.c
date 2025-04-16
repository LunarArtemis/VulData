static void FVMenuOblique(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    ObliqueDlg(fv,NULL);
}