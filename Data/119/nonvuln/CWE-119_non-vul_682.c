static void FVMenuEditInstrs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int index = FVAnyCharSelected(fv);
    SplineChar *sc;
    if ( index<0 )
return;
    sc = SFMakeChar(fv->b.sf,fv->b.map,index);
    SCEditInstructions(sc);
}