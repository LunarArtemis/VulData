static int gtextfield_focus(GGadget *g, GEvent *event) {
    GTextField *gt = (GTextField *) g;

    if ( g->state == gs_invisible || g->state == gs_disabled )
return( false );

    if ( gt->cursor!=NULL ) {
	GDrawCancelTimer(gt->cursor);
	gt->cursor = NULL;
	gt->cursor_on = false;
    }
    if ( gt->hidden_cursor && !event->u.focus.gained_focus ) {
	GDrawSetCursor(gt->g.base,gt->old_cursor);
	gt->hidden_cursor = false;
    }
    gt->g.has_focus = event->u.focus.gained_focus;
    if ( event->u.focus.gained_focus ) {
	gt->cursor = GDrawRequestTimer(gt->g.base,400,400,NULL);
	gt->cursor_on = true;
	if ( event->u.focus.mnemonic_focus != mf_normal )
	    GTextFieldSelect(&gt->g,0,-1);
	if ( gt->gic!=NULL )
	    GTPositionGIC(gt);
	else if ( GWidgetGetInputContext(gt->g.base)!=NULL )
	    GDrawSetGIC(gt->g.base,GWidgetGetInputContext(gt->g.base),10000,10000);
    }
    _ggadget_redraw(g);
    GTextFieldFocusChanged(gt,event->u.focus.gained_focus);
return( true );
}