static void FVMenuReplaceWithRef(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    FVReplaceOutlineWithReference(fv,.001);
}