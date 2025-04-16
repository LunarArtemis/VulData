static void FVMenuGlyphLabel(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    default_fv_glyphlabel = fv->glyphlabel = mi->mid;

    GDrawRequestExpose(fv->v,NULL,false);

    SavePrefs(true);
}