static int GS_OK(GGadget *g, GEvent *e) {

    if ( e->type==et_controlevent && e->u.control.subtype == et_buttonactivate ) {
	struct gsd *gs = GDrawGetUserData(GGadgetGetWindow(g));
	gs->done = true;
	gs->good = true;
    }
return( true );
}