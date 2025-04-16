static void FVMenuValidate(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SFValidationWindow(fv->b.sf,fv->b.active_layer,ff_none);
}