static int popup_eh(GWindow popup,GEvent *event) {
    GGadget *owner = GDrawGetUserData(popup);
    GTextField *gt = (GTextField *) owner;
    GCompletionField *gc = (GCompletionField *) owner;
    GRect old1, r;
    Color fg;
    int i, bp;

    if ( owner==NULL )		/* dying */
return( true );

    bp = GBoxBorderWidth(owner->base,owner->box);
    if ( event->type == et_expose ) {
	GDrawPushClip(popup,&event->u.expose.rect,&old1);
	GDrawSetFont(popup,gt->font);
	GBoxDrawBackground(popup,&event->u.expose.rect,owner->box,
		owner->state,false);
	GDrawGetSize(popup,&r);
	r.x = r.y = 0;
	GBoxDrawBorder(popup,&r,owner->box,owner->state,false);
	r.x += bp; r.width -= 2*bp;
	fg = owner->box->main_foreground==COLOR_DEFAULT?GDrawGetDefaultForeground(GDrawGetDisplayOfWindow(popup)):
		owner->box->main_foreground;
	for ( i=0; gc->choices[i]!=NULL; ++i ) {
	    if ( i==gc->selected ) {
		r.y = i*gt->fh+bp;
		r.height = gt->fh;
		GDrawFillRect(popup,&r,owner->box->active_border);
	    }
	    GDrawDrawText(popup,bp,i*gt->fh+gt->as+bp,gc->choices[i],-1,fg);
	}
	GDrawPopClip(popup,&old1);
    } else if ( event->type == et_mouseup ) {
	gc->selected = (event->u.mouse.y-bp)/gt->fh;
	if ( gc->selected>=0 && gc->selected<gc->ctot ) {
	    int tryagain = GTextFieldSetTitleRmDotDotDot(owner,gc->choices[gc->selected]);
	    GTextFieldChanged(gt,-1);
	    GCompletionDestroy(gc);
	    if ( tryagain )
		GTextFieldComplete(gt,false);
	} else {
	    gc->selected = -1;
	    GDrawRequestExpose(popup,NULL,false);
	}
    } else if ( event->type == et_char ) {
return( gtextfield_key(owner,event));
    }
return( true );
}