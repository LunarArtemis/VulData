GGadget *GSimpleListFieldCreate(struct gwindow *base, GGadgetData *gd,void *data) {
    GListField *ge = calloc(1,sizeof(GListField));

    ge->gt.listfield = true;
    if ( gd->u.list!=NULL )
	ge->ti = GTextInfoArrayFromList(gd->u.list,&ge->ltot);
    _GTextFieldCreate(&ge->gt,base,gd,data,&glistfield_box);
    ge->gt.g.funcs = &glistfield_funcs;
return( &ge->gt.g );
}