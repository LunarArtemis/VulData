static void FontView_Close(FontView *fv) {
    if ( fv->gw!=NULL )
	GDrawDestroyWindow(fv->gw);
    else
	FontViewRemove(fv);
}