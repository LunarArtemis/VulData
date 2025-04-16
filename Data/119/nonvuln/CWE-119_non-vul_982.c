static int gtextfield_hscroll(GGadget *g, GEvent *event) {
    enum sb sbt = event->u.control.u.sb.type;
    GTextField *gt = (GTextField *) (g->data);
    int xoff = gt->xoff_left;

    g = (GGadget *) gt;

    if ( sbt==et_sb_top )
	xoff = 0;
    else if ( sbt==et_sb_bottom ) {
	xoff = gt->xmax - gt->g.inner.width;
	if ( xoff<0 ) xoff = 0;
    } else if ( sbt==et_sb_up ) {
	if ( gt->xoff_left>gt->nw ) xoff = gt->xoff_left-gt->nw; else xoff = 0;
    } else if ( sbt==et_sb_down ) {
	if ( gt->xoff_left + gt->nw + gt->g.inner.width >= gt->xmax )
	    xoff = gt->xmax - gt->g.inner.width;
	else
	    xoff += gt->nw;
    } else if ( sbt==et_sb_uppage ) {
	int page = (3*g->inner.width)/4;
	xoff = gt->xoff_left - page;
	if ( xoff<0 ) xoff=0;
    } else if ( sbt==et_sb_downpage ) {
	int page = (3*g->inner.width)/4;
	xoff = gt->xoff_left + page;
	if ( xoff + gt->g.inner.width >= gt->xmax )
	    xoff = gt->xmax - gt->g.inner.width;
    } else /* if ( sbt==et_sb_thumb || sbt==et_sb_thumbrelease ) */ {
	xoff = event->u.control.u.sb.pos;
    }
    if ( xoff + gt->g.inner.width >= gt->xmax )
	xoff = gt->xmax - gt->g.inner.width;
    if ( xoff<0 ) xoff = 0;
    if ( gt->xoff_left!=xoff ) {
	gt->xoff_left = xoff;
	GScrollBarSetPos(&gt->hsb->g,xoff);
	_ggadget_redraw(&gt->g);
    }
return( true );
}