static int SS_Cancel(GGadget *g, GEvent *e) {

    if ( e->type==et_controlevent && e->u.control.subtype == et_buttonactivate ) {
	int *done = GDrawGetUserData(GGadgetGetWindow(g));
	*done = true;
    }
return( true );
}