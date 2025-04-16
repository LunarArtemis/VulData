static void FVMenuOutline(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    OutlineDlg(fv,NULL,NULL,false);
}