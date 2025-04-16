static void FVMenuShowGroup(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    ShowGroup( ((FontView *) GDrawGetUserData(gw))->sf );
}