static int v_e_h(GWindow gw, GEvent *event) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if (( event->type==et_mouseup || event->type==et_mousedown ) &&
	    (event->u.mouse.button>=4 && event->u.mouse.button<=7) ) {
return( GGadgetDispatchEvent(fv->vsb,event));
    }

    GGadgetPopupExternalEvent(event);
    switch ( event->type ) {
      case et_expose:
	GDrawSetLineWidth(gw,0);
	FVExpose(fv,gw,event);
      break;
      case et_char:
	if ( fv->b.container!=NULL )
	    (fv->b.container->funcs->charEvent)(fv->b.container,event);
	else
	    FVChar(fv,event);
      break;
      case et_mousemove: case et_mousedown: case et_mouseup:
	if ( event->type==et_mousedown )
	    GDrawSetGIC(gw,fv->gic,0,20);
	if ( fv->notactive && event->type==et_mousedown )
	    (fv->b.container->funcs->activateMe)(fv->b.container,&fv->b);
	FVMouse(fv,event);
      break;
      case et_timer:
	FVTimer(fv,event);
      break;
      case et_focus:
//	  printf("fv.et_focus\n");
	if ( event->u.focus.gained_focus )
	    GDrawSetGIC(gw,fv->gic,0,20);
      break;
    }
return( true );
}