static GRect *gtextfield_getsize(GGadget *g, GRect *r ) {
    GTextField *gt = (GTextField *) g;
    _ggadget_getsize(g,r);
    if ( gt->vsb!=NULL )
	r->width =  gt->vsb->g.r.x+gt->vsb->g.r.width-g->r.x;
    if ( gt->hsb!=NULL )
	r->height =  gt->hsb->g.r.y+gt->hsb->g.r.height-g->r.y;
return( r );
}