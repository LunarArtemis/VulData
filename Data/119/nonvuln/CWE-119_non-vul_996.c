static int gnumericfield_mouse(GTextField *gt, GEvent *event) {
    GListField *ge = (GListField *) gt;
    if ( event->type==et_mousedown ) {
	gt->incr_down = event->u.mouse.y > (ge->buttonrect.y + ge->buttonrect.height/2);
	GTextFieldIncrement(gt,gt->incr_down?-1:1);
	if ( gt->numeric_scroll==NULL )
	    gt->numeric_scroll = GDrawRequestTimer(gt->g.base,200,100,NULL);
    } else if ( gt->numeric_scroll!=NULL ) {
	GDrawCancelTimer(gt->numeric_scroll);
	gt->numeric_scroll = NULL;
    }
return( true );
}