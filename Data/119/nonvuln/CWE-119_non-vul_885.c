static void FVMenuUnlinkRef(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVUnlinkRef( (FontViewBase *) fv );
}