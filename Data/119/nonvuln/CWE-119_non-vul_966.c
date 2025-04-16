static void GTextFieldFit(GTextField *gt) {
    GTextBounds bounds;
    int as=0, ds, ld, width=0;
    GRect inner, outer;
    int bp = GBoxBorderWidth(gt->g.base,gt->g.box);

    {
	FontInstance *old = GDrawSetFont(gt->g.base,gt->font);
	FontRequest rq;
	int tries;
	for ( tries = 0; tries<2; ++tries ) {
	    width = GDrawGetTextBounds(gt->g.base,gt->text, -1, &bounds);
	    GDrawWindowFontMetrics(gt->g.base,gt->font,&as, &ds, &ld);
	    if ( gt->g.r.height==0 || as+ds-3+2*bp<=gt->g.r.height || tries==1 )
	break;
	    /* Doesn't fit. Try a smaller size */
	    GDrawDecomposeFont(gt->font,&rq);
	    --rq.point_size;
	    gt->font = GDrawInstanciateFont(gt->g.base,&rq);
	}
	gt->fh = as+ds;
	gt->as = as;
	gt->nw = GDrawGetTextWidth(gt->g.base,nstr, 1);
	GDrawSetFont(gt->g.base,old);
    }

    GTextFieldGetDesiredSize(&gt->g,&outer,&inner);
    if ( gt->g.r.width==0 ) {
	int extra=0;
	if ( gt->listfield ) {
	    extra = GDrawPointsToPixels(gt->g.base,_GListMarkSize) +
		    2*GDrawPointsToPixels(gt->g.base,_GGadget_TextImageSkip) +
		    GBoxBorderWidth(gt->g.base,&_GListMark_Box);
	} else if ( gt->numericfield ) {
	    extra = GDrawPointsToPixels(gt->g.base,_GListMarkSize)/2 +
		    GDrawPointsToPixels(gt->g.base,_GGadget_TextImageSkip) +
		    2*GBoxBorderWidth(gt->g.base,&gnumericfieldspinner_box);
	}
	gt->g.r.width = outer.width;
	gt->g.inner.width = inner.width;
	gt->g.inner.x = gt->g.r.x + (outer.width-inner.width-extra)/2;
    } else {
	gt->g.inner.x = gt->g.r.x + bp;
	gt->g.inner.width = gt->g.r.width - 2*bp;
    }
    if ( gt->g.r.height==0 ) {
	gt->g.r.height = outer.height;
	gt->g.inner.height = inner.height;
	gt->g.inner.y = gt->g.r.y + (outer.height-gt->g.inner.height)/2;
    } else {
	gt->g.inner.y = gt->g.r.y + bp;
	gt->g.inner.height = gt->g.r.height - 2*bp;
    }

    if ( gt->multi_line ) {
	GTextFieldAddVSb(gt);
	if ( !gt->wrap )
	    GTextFieldAddHSb(gt);
    }
    if ( gt->listfield || gt->numericfield ) {
	GListField *ge = (GListField *) gt;
	int extra;
	if ( gt->listfield )
	    extra = GDrawPointsToPixels(gt->g.base,_GListMarkSize) +
		    GDrawPointsToPixels(gt->g.base,_GGadget_TextImageSkip) +
		    2*GBoxBorderWidth(gt->g.base,&_GListMark_Box)+
		    GBoxBorderWidth(gt->g.base,&glistfieldmenu_box);
	else {
	    extra = GDrawPointsToPixels(gt->g.base,_GListMarkSize)/2 +
		    GDrawPointsToPixels(gt->g.base,_GGadget_TextImageSkip) +
		    2*GBoxBorderWidth(gt->g.base,&gnumericfieldspinner_box);
	}
	ge->fieldrect = ge->buttonrect = gt->g.r;
	ge->fieldrect.width -= extra;
	extra -= GDrawPointsToPixels(gt->g.base,_GGadget_TextImageSkip)/2;
	ge->buttonrect.x = ge->buttonrect.x+ge->buttonrect.width-extra;
	ge->buttonrect.width = extra;
	if ( gt->numericfield )
	    ++ge->fieldrect.width;
    }
}