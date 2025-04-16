GGadget *GNumericFieldCreate(struct gwindow *base, GGadgetData *gd,void *data) {
    GTextField *gt = calloc(1,sizeof(GNumericField));
    gt->numericfield = true;
    _GTextFieldCreate(gt,base,gd,data,&gnumericfield_box);

return( &gt->g );
}