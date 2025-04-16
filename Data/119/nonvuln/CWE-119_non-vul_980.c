GGadget *GTextAreaCreate(struct gwindow *base, GGadgetData *gd,void *data) {
    GTextField *gt = calloc(1,sizeof(GTextField));
    gt->multi_line = true;
    gt->accepts_returns = true;
    _GTextFieldCreate(gt,base,gd,data,&_GGadget_gtextfield_box);

return( &gt->g );
}