static void FVMenuJustify(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *sf = fv->b.cidmaster == NULL ? fv->b.sf : fv->b.cidmaster;
    JustifyDlg(sf);
}