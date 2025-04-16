static void FVMenuNLTransform(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    if ( FVAnyCharSelected(fv)==-1 )
return;
    NonLinearDlg(fv,NULL);
}