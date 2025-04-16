static void FVMenuForceEncode(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    Encoding *enc = NULL;
    int oldcnt = fv->b.map->enccount;

    enc = FindOrMakeEncoding(mi->ti.userdata);
    if ( enc==NULL ) {
	IError("Known encoding could not be found");
return;
    }
    SFForceEncoding(fv->b.sf,fv->b.map,enc);
    if ( oldcnt < fv->b.map->enccount ) {
	fv->b.selected = realloc(fv->b.selected,fv->b.map->enccount);
	memset(fv->b.selected+oldcnt,0,fv->b.map->enccount-oldcnt);
    }
    if ( fv->b.normal!=NULL ) {
	EncMapFree(fv->b.normal);
	if (fv->b.normal == fv->b.sf->map) { fv->b.sf->map = NULL; }
	fv->b.normal = NULL;
    }
    SFReplaceEncodingBDFProps(fv->b.sf,fv->b.map);
    FontViewSetTitle(fv);
    FontViewReformatOne(&fv->b);
}