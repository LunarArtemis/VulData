static void FVMenuEmbolden(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    EmboldenDlg(fv,NULL);
}