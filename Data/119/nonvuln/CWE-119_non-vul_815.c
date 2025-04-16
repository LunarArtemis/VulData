static int gs_sub_e_h(GWindow pixmap, GEvent *event) {
    FontView *active_fv;
    struct gsd *gs;

    if ( event->type==et_destroy )
return( true );

    active_fv = (FontView *) GDrawGetUserData(pixmap);
    gs = (struct gsd *) (active_fv->b.container);

    if (( event->type==et_mouseup || event->type==et_mousedown ) &&
	    (event->u.mouse.button>=4 && event->u.mouse.button<=7) ) {
return( GGadgetDispatchEvent(active_fv->vsb,event));
    }


    switch ( event->type ) {
      case et_expose:
	FVDrawInfo(active_fv,pixmap,event);
      break;
      case et_char:
	gs_charEvent(&gs->base,event);
      break;
      case et_mousedown:
return(false);
      break;
      case et_mouseup: case et_mousemove:
return(false);
      case et_resize:
        gs_sizeSet(gs,pixmap);
      break;
    }
return( true );
}