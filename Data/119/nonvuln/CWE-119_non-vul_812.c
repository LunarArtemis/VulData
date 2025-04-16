static void gs_sizeSet(struct gsd *gs,GWindow dw) {
    GRect size, gsize;
    int width, height, y;
    int cc, rc, topchar;
    GRect subsize;
    FontView *fv = gs->fv;

    if ( gs->fv->vsb==NULL )
return;

    GDrawGetSize(dw,&size);
    GGadgetGetSize(gs->fv->vsb,&gsize);
    width = size.width - gsize.width;
    height = size.height - gs->fv->mbh - gs->fv->infoh;

    y = gs->fv->mbh + gs->fv->infoh;

    topchar = fv->rowoff*fv->colcnt;
    cc = (width-1) / fv->cbw;
    if ( cc<1 ) cc=1;
    rc = (height-1)/ fv->cbh;
    if ( rc<1 ) rc = 1;
    subsize.x = 0; subsize.y = 0;
    subsize.width = cc*fv->cbw + 1;
    subsize.height = rc*fv->cbh + 1;
    GDrawResize(fv->v,subsize.width,subsize.height);
    GDrawMove(fv->v,0,y);
    GGadgetMove(fv->vsb,subsize.width,y);
    GGadgetResize(fv->vsb,gsize.width,subsize.height);

    fv->colcnt = cc; fv->rowcnt = rc;
    fv->width = subsize.width; fv->height = subsize.height;
    fv->rowltot = (fv->b.map->enccount+fv->colcnt-1)/fv->colcnt;
    GScrollBarSetBounds(fv->vsb,0,fv->rowltot,fv->rowcnt);
    fv->rowoff = topchar/fv->colcnt;
    if ( fv->rowoff>=fv->rowltot-fv->rowcnt )
        fv->rowoff = fv->rowltot-fv->rowcnt;
    if ( fv->rowoff<0 ) fv->rowoff =0;
    GScrollBarSetPos(fv->vsb,fv->rowoff);

    GDrawRequestExpose(fv->v,NULL,true);
}