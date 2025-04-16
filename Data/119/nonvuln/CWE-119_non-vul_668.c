static void FVMenuChangeGlyph(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    GlyphChangeDlg(fv,NULL,gc_generic);
}