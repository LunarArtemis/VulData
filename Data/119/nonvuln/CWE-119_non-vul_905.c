static int GS_Cancel(GGadget *g, GEvent *e) {
    struct gsd *gs;

    if ( e->type==et_controlevent && e->u.control.subtype == et_buttonactivate ) {
	gs = GDrawGetUserData(GGadgetGetWindow(g));
	gs->done = true;
    }
return( true );
}