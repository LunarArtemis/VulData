static void FVMenuRound2Hundredths(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVRound2Int( (FontViewBase *) GDrawGetUserData(gw),100.0 );
}