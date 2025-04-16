static void FVMenuRevertGlyph(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FVRevertGlyph((FontViewBase *) fv);
}