static void GTextFieldGetDesiredSize(GGadget *g,GRect *outer,GRect *inner) {
    GTextField *gt = (GTextField *) g;
    int width=0, height;
    int extra=0;
    int bp = GBoxBorderWidth(g->base,g->box);

    if ( gt->listfield ) {
	extra = GDrawPointsToPixels(gt->g.base,_GListMarkSize) +
		GDrawPointsToPixels(gt->g.base,_GGadget_TextImageSkip) +
		2*GBoxBorderWidth(gt->g.base,&_GListMark_Box) +
		GBoxBorderWidth(gt->g.base,&glistfieldmenu_box);
    } else if ( gt->numericfield ) {
	extra = GDrawPointsToPixels(gt->g.base,_GListMarkSize)/2 +
		GDrawPointsToPixels(gt->g.base,_GGadget_TextImageSkip) +
		2*GBoxBorderWidth(gt->g.base,&gnumericfieldspinner_box);
    }

    width = GGadgetScale(GDrawPointsToPixels(gt->g.base,80));
    height = gt->multi_line? 4*gt->fh:gt->fh;

    if ( g->desired_width>extra+2*bp ) width = g->desired_width - extra - 2*bp;
    if ( g->desired_height>2*bp ) height = g->desired_height - 2*bp;

    if ( gt->multi_line ) {
	int sbadd = GDrawPointsToPixels(gt->g.base,_GScrollBar_Width) +
		GDrawPointsToPixels(gt->g.base,1);
	width += sbadd;
	if ( !gt->wrap )
	    height += sbadd;
    }

    if ( inner!=NULL ) {
	inner->x = inner->y = 0;
	inner->width = width;
	inner->height = height;
    }
    if ( outer!=NULL ) {
	outer->x = outer->y = 0;
	outer->width = width + extra + 2*bp;
	outer->height = height + 2*bp;
    }
}