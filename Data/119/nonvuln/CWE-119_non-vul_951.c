GGadget *GPasswordCreate(struct gwindow *base, GGadgetData *gd,void *data) {
    GTextField *gt = _GTextFieldCreate(calloc(1,sizeof(GTextField)),base,gd,data,&_GGadget_gtextfield_box);
    gt->password = true;
    GTextFieldRefigureLines(gt, 0);

return( &gt->g );
}