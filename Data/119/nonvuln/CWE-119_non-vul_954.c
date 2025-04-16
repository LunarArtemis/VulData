static void gtextfield_setvisible(GGadget *g, int visible ) {
    GTextField *gt = (GTextField *) g;
    if ( gt->vsb!=NULL ) _ggadget_setvisible(&gt->vsb->g,visible);
    if ( gt->hsb!=NULL ) _ggadget_setvisible(&gt->hsb->g,visible);
    _ggadget_setvisible(g,visible);
}