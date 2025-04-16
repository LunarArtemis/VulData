static void FVMenuPasteAfter(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int pos = FVAnyCharSelected(fv);
    if ( pos<0 )
return;
    PasteIntoFV(&fv->b,2,NULL);
}