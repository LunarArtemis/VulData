static int gs_e_h(GWindow gw, GEvent *event) {
    struct gsd *gs = GDrawGetUserData(gw);

    switch ( event->type ) {
      case et_close:
	gs->done = true;
      break;
      case et_char:
	FVChar(gs->fv,event);
      break;
    }
return( true );
}