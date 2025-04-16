static void FVMenuAddExtrema(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVAddExtrema( (FontViewBase *) GDrawGetUserData(gw) , false);
}