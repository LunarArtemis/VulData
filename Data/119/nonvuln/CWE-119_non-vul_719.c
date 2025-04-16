static void FVMenuRemoveVKern(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    FVRemoveVKerns(&fv->b);
}