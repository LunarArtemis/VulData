static void gtextfield_setenabled(GGadget *g, int enabled ) {
    GTextField *gt = (GTextField *) g;
    if ( gt->vsb!=NULL ) _ggadget_setenabled(&gt->vsb->g,enabled);
    if ( gt->hsb!=NULL ) _ggadget_setenabled(&gt->hsb->g,enabled);
    _ggadget_setenabled(g,enabled);
}