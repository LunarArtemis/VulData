static void FVMenuSimplify(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVSimplify( (FontView *) GDrawGetUserData(gw),false );
}