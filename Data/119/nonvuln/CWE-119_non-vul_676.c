static void FVMenuCluster(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVCluster( (FontViewBase *) GDrawGetUserData(gw));
}