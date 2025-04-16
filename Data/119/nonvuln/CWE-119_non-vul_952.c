static void GTextFieldDrawDDCursor(GTextField *gt, int pos) {
    GRect old;
    int x, y, l;

    l = GTextFieldFindLine(gt,pos);
    if ( l<gt->loff_top || l>=gt->loff_top + (gt->g.inner.height/gt->fh))
return;
    _gt_cursor_pos(gt,pos,&x,&y);
    if ( x<0 || x>=gt->g.inner.width )
return;

    GDrawPushClip(gt->g.base,&gt->g.inner,&old);
    GDrawSetDifferenceMode(gt->g.base);
    GDrawSetFont(gt->g.base,gt->font);
    GDrawSetLineWidth(gt->g.base,0);
    GDrawSetDashedLine(gt->g.base,2,2,0);
    GDrawDrawLine(gt->g.base,gt->g.inner.x+x,gt->g.inner.y+y,
	    gt->g.inner.x+x,gt->g.inner.y+y+gt->fh,
	    COLOR_WHITE);
    GDrawPopClip(gt->g.base,&old);
    GDrawSetDashedLine(gt->g.base,0,0,0);
    gt->has_dd_cursor = !gt->has_dd_cursor;
    gt->dd_cursor_pos = pos;
}