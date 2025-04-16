static void FVMenuCharInfo(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int pos = FVAnyCharSelected(fv);
    if ( pos<0 )
return;
    if ( fv->b.cidmaster!=NULL &&
	    (fv->b.map->map[pos]==-1 || fv->b.sf->glyphs[fv->b.map->map[pos]]==NULL ))
return;
    SCCharInfo(SFMakeChar(fv->b.sf,fv->b.map,pos),fv->b.active_layer,fv->b.map,pos);
}