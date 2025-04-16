static void FVMenuBDFInfo(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    if ( fv->b.sf->bitmaps==NULL )
return;
    if ( fv->show!=fv->filled )
	SFBdfProperties(fv->b.sf,fv->b.map,fv->show);
    else
	SFBdfProperties(fv->b.sf,fv->b.map,NULL);
}