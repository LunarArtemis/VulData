static void FVMenuRound2Int(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVRound2Int( (FontViewBase *) GDrawGetUserData(gw), 1.0 );
}