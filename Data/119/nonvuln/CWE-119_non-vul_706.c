static void FVMenuVKernFromHKern(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    FVVKernFromHKern(&fv->b);
}