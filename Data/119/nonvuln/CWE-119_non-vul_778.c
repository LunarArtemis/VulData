static void FVMenuCorrectRefs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontViewBase *fv = (FontViewBase *) GDrawGetUserData(gw);

    FVCorrectReferences(fv);
}