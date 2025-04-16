static void FVMenuShowDependentRefs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int pos = FVAnyCharSelected(fv);
    SplineChar *sc;

    if ( pos<0 || fv->b.map->map[pos]==-1 )
return;
    sc = fv->b.sf->glyphs[fv->b.map->map[pos]];
    if ( sc==NULL || sc->dependents==NULL )
return;
    SCRefBy(sc);
}