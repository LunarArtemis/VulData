static int gtextfield_sel(GGadget *g, GEvent *event) {
    GTextField *gt = (GTextField *) g;
    unichar_t *end;
    int i;

    if ( event->type == et_selclear ) {
	if ( event->u.selclear.sel==sn_primary && gt->sel_start!=gt->sel_end ) {
	    gt->sel_start = gt->sel_end = gt->sel_base;
	    _ggadget_redraw(g);
            return( true );
	}
        return( false );
    }

    if ( gt->has_dd_cursor )
	GTextFieldDrawDDCursor(gt,gt->dd_cursor_pos);
    GDrawSetFont(g->base,gt->font);
    i = (event->u.drag_drop.y-g->inner.y)/gt->fh + gt->loff_top;
    if ( !gt->multi_line ) i = 0;
    if ( i>=gt->lcnt )
	end = gt->text+u_strlen(gt->text);
    else
	end = GTextFieldGetPtFromPos(gt,i,event->u.drag_drop.x);
    if ( event->type == et_drag ) {
	GTextFieldDrawDDCursor(gt,end-gt->text);
    } else if ( event->type == et_dragout ) {
	/* this event exists simply to clear the dd cursor line. We've done */
	/*  that already */ 
    } else if ( event->type == et_drop ) {
	gt->sel_start = gt->sel_end = gt->sel_base = end-gt->text;
	GTextFieldPaste(gt,sn_drag_and_drop);
	GTextField_Show(gt,gt->sel_start);
	GTextFieldChanged(gt,-1);
	_ggadget_redraw(&gt->g);
    } else
return( false );

return( true );
}