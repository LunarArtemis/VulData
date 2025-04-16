static int glistfield_mouse(GListField *ge, GEvent *event) {
    if ( event->type!=et_mousedown )
return( true );
    if ( ge->popup != NULL ) {
	GDrawDestroyWindow(ge->popup);
	ge->popup = NULL;
return( true );
    }
    ge->popup = GListPopupCreate(&ge->gt.g,GListFieldSelected,ge->ti);
return( true );
}