static int GCompletionHandleKey(GTextField *gt,GEvent *event) {
    GCompletionField *gc = (GCompletionField *) gt;
    int dir = 0;

    if ( gc->choice_popup==NULL || event->type == et_charup )
return( false );

    if ( event->u.chr.keysym == GK_Up || event->u.chr.keysym == GK_KP_Up )
	dir = -1;
    else if ( event->u.chr.keysym == GK_Down || event->u.chr.keysym == GK_KP_Down )
	dir = 1;

    if ( dir==0 || event->u.chr.chars[0]!='\0' ) {
	/* For normal characters we destroy the popup window and pretend it */
	/*  wasn't there */
	GCompletionDestroy(gc);
	if ( event->u.chr.keysym == GK_Escape )
	    gt->was_completing = false;
return( event->u.chr.keysym == GK_Escape ||	/* Eat an escape, other chars will be processed further */
	    event->u.chr.keysym == GK_Return );
    }

    if (( gc->selected==-1 && dir==-1 ) || ( gc->selected==gc->ctot-1 && dir==1 ))
return( true );
    gc->selected += dir;
    if ( gc->selected!=-1 )
	GTextFieldSetTitleRmDotDotDot(&gt->g,gc->choices[gc->selected]);
    GTextFieldChanged(gt,-1);
    GDrawRequestExpose(gc->choice_popup,NULL,false);
return( true );
}