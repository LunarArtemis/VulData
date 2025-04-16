static void FVMenuCanonicalContours(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FVCanonicalContours( (FontViewBase *) GDrawGetUserData(gw) );
}