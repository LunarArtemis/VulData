static void FVMenuCopyFgBg(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVCopyFgtoBg( (FontViewBase *) fv );
}