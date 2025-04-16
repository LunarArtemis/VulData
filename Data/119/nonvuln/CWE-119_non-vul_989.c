static void gtextfield_move(GGadget *g, int32 x, int32 y ) {
    GTextField *gt = (GTextField *) g;
    int fxo=0, fyo=0, bxo, byo;

    if ( gt->listfield || gt->numericfield ) {
	fxo = ((GListField *) gt)->fieldrect.x - g->r.x;
	fyo = ((GListField *) gt)->fieldrect.y - g->r.y;
	bxo = ((GListField *) gt)->buttonrect.x - g->r.x;
	byo = ((GListField *) gt)->buttonrect.y - g->r.y;
    }
    if ( gt->vsb!=NULL )
	_ggadget_move((GGadget *) (gt->vsb),x+(gt->vsb->g.r.x-g->r.x),y);
    if ( gt->hsb!=NULL )
	_ggadget_move((GGadget *) (gt->hsb),x,y+(gt->hsb->g.r.y-g->r.y));
    _ggadget_move(g,x,y);
    if ( gt->listfield || gt->numericfield ) {
	((GListField *) gt)->fieldrect.x = g->r.x + fxo;
	((GListField *) gt)->fieldrect.y = g->r.y + fyo;
	((GListField *) gt)->buttonrect.x = g->r.x + bxo;
	((GListField *) gt)->buttonrect.y = g->r.y + byo;
    }
}