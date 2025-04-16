static void FVMenuSimplifyMore(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVSimplify( (FontView *) GDrawGetUserData(gw),true );
}