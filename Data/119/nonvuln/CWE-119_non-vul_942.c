static void FVMenuRevertBackup(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontViewBase *fv = (FontViewBase *) GDrawGetUserData(gw);
    FVRevertBackup(fv);
}