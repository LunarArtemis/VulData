static void FVMenuGenerateTTC(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    _FVMenuGenerate(fv,gf_ttc);
}