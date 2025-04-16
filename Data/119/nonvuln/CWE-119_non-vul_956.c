static int gtextfield_key(GGadget *g, GEvent *event) {
    GTextField *gt = (GTextField *) g;

    if ( !g->takes_input || (g->state!=gs_enabled && g->state!=gs_active && g->state!=gs_focused ))
return( false );
    if ( gt->listfield && ((GListField *) gt)->popup!=NULL ) {
	GWindow popup = ((GListField *) gt)->popup;
	(GDrawGetEH(popup))(popup,event);
return( true );
    }

    if ( gt->completionfield && ((GCompletionField *) gt)->choice_popup!=NULL &&
	    GCompletionHandleKey(gt,event))
return( true );

    if ( event->type == et_charup )
return( false );
    if ( event->u.chr.keysym == GK_F1 || event->u.chr.keysym == GK_Help ||
	    (event->u.chr.keysym == GK_Return && !gt->accepts_returns ) ||
	    ( event->u.chr.keysym == GK_Tab && !gt->accepts_tabs ) ||
	    event->u.chr.keysym == GK_BackTab || event->u.chr.keysym == GK_Escape )
return( false );

    if ( !gt->hidden_cursor ) {	/* hide the mouse pointer */
	if ( !gt->drag_and_drop )
	    gt->old_cursor = GDrawGetCursor(gt->g.base);
	GDrawSetCursor(g->base,ct_invisible);
	gt->hidden_cursor = true;
	_GWidget_SetGrabGadget(g);	/* so that we get the next mouse movement to turn the cursor on */
    }
    if( gt->cursor_on ) {	/* undraw the blinky text cursor if it is drawn */
	gt_draw_cursor(g->base, gt);
	gt->cursor_on = false;
    }

    switch ( GTextFieldDoChange(gt,event)) {
      case 4:
	/* We should try name completion */
	if ( gt->completionfield && ((GCompletionField *) gt)->completion!=NULL &&
		gt->was_completing && gt->sel_start == u_strlen(gt->text))
	    GTextFieldComplete(gt,false);
	else
	    GTextFieldChanged(gt,-1);
      break;
      case 3:
	/* They typed a Tab */
      break;
      case 2:
      break;
      case true:
	GTextFieldChanged(gt,-1);
      break;
      case false:
return( false );
    }
    _ggadget_redraw(g);
return( true );
}