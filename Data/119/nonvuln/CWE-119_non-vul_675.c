static void FVMenuCleanup(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVSimplify( (FontView *) GDrawGetUserData(gw),-1 );
}