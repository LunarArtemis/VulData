static void FVMenuCanonicalStart(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVCanonicalStart( (FontViewBase *) GDrawGetUserData(gw) );
}