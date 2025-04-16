static void FVMenuMakeFromFont(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    (void) MakeEncoding(fv->b.sf,fv->b.map);
}