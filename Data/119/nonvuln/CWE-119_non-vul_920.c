static int fv_e_h(GWindow gw, GEvent *event) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if (( event->type==et_mouseup || event->type==et_mousedown ) &&
	    (event->u.mouse.button>=4 && event->u.mouse.button<=7) ) {
return( GGadgetDispatchEvent(fv->vsb,event));
    }

    switch ( event->type ) {
      case et_focus:
	  if ( event->u.focus.gained_focus )
	  {
	      ActiveFontView = fv;
	  }
	  else
	  {
	  }
	  break;
      case et_selclear:
#ifdef __Mac
	  // For some reason command + c and command + x wants
	  // to send a clear to us, even if that key was pressed
	  // on a charview.
	  if( osx_fontview_copy_cut_counter )
	  {
	     osx_fontview_copy_cut_counter--;
	     break;
          }
//	  printf("fontview et_selclear\n");
#endif
	ClipboardClear();
      break;
      case et_expose:
	GDrawSetLineWidth(gw,0);
	FVDrawInfo(fv,gw,event);
      break;
      case et_resize:
	/* KDE sends a continuous stream of resize events, and gets very */
	/*  confused if I start resizing the window myself, try to wait for */
	/*  the user to finish before responding to resizes */
	if ( event->u.resize.sized || fv->resize_expected ) {
	    if ( fv->resize )
		GDrawCancelTimer(fv->resize);
	    fv->resize_event = *event;
	    fv->resize = GDrawRequestTimer(fv->v,300,0,(void *) &fv->resize_event);
	    fv->resize_expected = false;
	}
      break;
      case et_char:
	if ( fv->b.container!=NULL )
	    (fv->b.container->funcs->charEvent)(fv->b.container,event);
	else
	    FVChar(fv,event);
      break;
      case et_mousedown:
	GDrawSetGIC(gw,fv->gwgic,0,20);
	if ( fv->notactive )
	    (fv->b.container->funcs->activateMe)(fv->b.container,&fv->b);
      break;
      case et_close:
	FVMenuClose(gw,NULL,NULL);
      break;
      case et_create:
	fv->b.next = (FontViewBase *) fv_list;
	fv_list = fv;
      break;
      case et_destroy:
	if ( fv->qg!=NULL )
	    QGRmFontView(fv->qg,fv);
	FontViewRemove(fv);
      break;
    }
return( true );
}