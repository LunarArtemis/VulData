static int gtextfield_expose(GWindow pixmap, GGadget *g, GEvent *event) {
    GTextField *gt = (GTextField *) g;
    GListField *ge = (GListField *) g;
    GRect old1, old2, *r = &g->r;
    Color fg;
    int ll,i, last;
    GRect unpadded_inner;
    int pad;

    if ( g->state == gs_invisible || gt->dontdraw )
return( false );

    if ( gt->listfield || gt->numericfield ) r = &ge->fieldrect;

    GDrawPushClip(pixmap,r,&old1);

    GBoxDrawBackground(pixmap,r,g->box,
	    g->state==gs_enabled? gs_pressedactive: g->state,false);
    GBoxDrawBorder(pixmap,r,g->box,g->state,false);

    unpadded_inner = g->inner;
    pad = GDrawPointsToPixels(g->base,g->box->padding);
    unpadded_inner.x -= pad; unpadded_inner.y -= pad;
    unpadded_inner.width += 2*pad; unpadded_inner.height += 2*pad;
    GDrawPushClip(pixmap,&unpadded_inner,&old2);
    GDrawSetFont(pixmap,gt->font);

    fg = g->state==gs_disabled?g->box->disabled_foreground:
		    g->box->main_foreground==COLOR_DEFAULT?GDrawGetDefaultForeground(GDrawGetDisplayOfWindow(pixmap)):
		    g->box->main_foreground;
    ll = 0;
    if ( (last = gt->g.inner.height/gt->fh)==0 ) last = 1;
    if ( gt->sel_start != gt->sel_end ) {
	/* I used to have support for drawing on a bw display where the */
	/*  selection and the foreground color were the same (black) and */
	/*  selected text was white. No longer. */
	/* Draw the entire selection first, then the text itself */
	for ( i=gt->loff_top; i<gt->loff_top+last && gt->lines[i]!=-1; ++i ) {
	    if ( gt->sel_end>gt->lines[i] &&
		    (gt->lines[i+1]==-1 || gt->sel_start<gt->lines[i+1]))
		GTextFieldDrawLineSel(pixmap,gt,i);
	}
    }
    for ( i=gt->loff_top; i<gt->loff_top+last && gt->lines[i]!=-1; ++i )
	GTextFieldDrawLine(pixmap,gt,i,fg);

    GDrawPopClip(pixmap,&old2);
    GDrawPopClip(pixmap,&old1);
    gt_draw_cursor(pixmap, gt);

    if ( gt->listfield ) {
	int marklen = GDrawPointsToPixels(pixmap,_GListMarkSize);

	GDrawPushClip(pixmap,&ge->buttonrect,&old1);

	GBoxDrawBackground(pixmap,&ge->buttonrect,&glistfieldmenu_box,
		g->state==gs_enabled? gs_pressedactive: g->state,false);
	GBoxDrawBorder(pixmap,&ge->buttonrect,&glistfieldmenu_box,g->state,false);

	GListMarkDraw(pixmap,
		ge->buttonrect.x + (ge->buttonrect.width - marklen)/2,
		g->inner.y,
		g->inner.height,
		g->state);
	GDrawPopClip(pixmap,&old1);
    } else if ( gt->numericfield ) {
	int y, w;
	int half;
	GPoint pts[5];
	int bp = GBoxBorderWidth(gt->g.base,&gnumericfieldspinner_box);
	Color fg = g->state==gs_disabled?gnumericfieldspinner_box.disabled_foreground:
			gnumericfieldspinner_box.main_foreground==COLOR_DEFAULT?GDrawGetDefaultForeground(GDrawGetDisplayOfWindow(pixmap)):
			gnumericfieldspinner_box.main_foreground;

	GBoxDrawBackground(pixmap,&ge->buttonrect,&gnumericfieldspinner_box,
		g->state==gs_enabled? gs_pressedactive: g->state,false);
	GBoxDrawBorder(pixmap,&ge->buttonrect,&gnumericfieldspinner_box,g->state,false);
	/* GDrawDrawRect(pixmap,&ge->buttonrect,fg); */

	y = ge->buttonrect.y + ge->buttonrect.height/2;
	w = ge->buttonrect.width;
	w &= ~1;
	pts[0].x = ge->buttonrect.x+3+bp;
	pts[1].x = ge->buttonrect.x+w-3-bp;
	pts[2].x = ge->buttonrect.x + w/2;
	half = pts[2].x-pts[0].x;
	GDrawDrawLine(pixmap, pts[0].x-3,y, pts[1].x+3,y, fg );
	pts[0].y = pts[1].y = y-2;
	pts[2].y = pts[1].y-half;
	pts[3] = pts[0];
	GDrawFillPoly(pixmap,pts,3,fg);
	pts[0].y = pts[1].y = y+2;
	pts[2].y = pts[1].y+half;
	pts[3] = pts[0];
	GDrawFillPoly(pixmap,pts,3,fg);
    }
return( true );
}