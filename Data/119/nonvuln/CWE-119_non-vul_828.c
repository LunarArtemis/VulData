void KFFontViewInits(struct kf_dlg *kf,GGadget *drawable) {
    GGadgetData gd;
    GRect pos, gsize, sbsize;
    GWindow dw = GDrawableGetWindow(drawable);
    int infoh;
    int ps;
    FontView *fvorig = (FontView *) kf->sf->fv;

    FontViewInit();

    kf->dw = dw;

    memset(&gd,0,sizeof(gd));
    gd.flags = gg_visible | gg_enabled;
    helplist[0].invoke = FVMenuContextualHelp;
    gd.u.menu2 = mblist;
    kf->mb = GMenu2BarCreate( dw, &gd, NULL);
    GGadgetGetSize(kf->mb,&gsize);
    kf->mbh = gsize.height;
    kf->guts = drawable;

    ps = kf->sf->display_size; kf->sf->display_size = -24;
    kf->first_fv = __FontViewCreate(kf->sf); kf->first_fv->b.container = (struct fvcontainer *) kf;
    kf->second_fv = __FontViewCreate(kf->sf); kf->second_fv->b.container = (struct fvcontainer *) kf;

    kf->infoh = infoh = 1+GDrawPointsToPixels(NULL,fv_fontsize);
    kf->first_fv->mbh = kf->mbh;
    pos.x = 0; pos.y = kf->mbh+infoh+kf->fh+4;
    pos.width = 16*kf->first_fv->cbw+1;
    pos.height = 4*kf->first_fv->cbh+1;

    GDrawSetUserData(dw,kf->first_fv);
    FVCopyInnards(kf->first_fv,&pos,infoh,fvorig,dw,kf->def_layer,(struct fvcontainer *) kf);
    pos.height = 4*kf->first_fv->cbh+1;		/* We don't know the real fv->cbh until after creating the innards. The size of the last window is probably wrong, we'll fix later */
    kf->second_fv->mbh = kf->mbh;
    kf->label2_y = pos.y + pos.height+2;
    pos.y = kf->label2_y + kf->fh + 2;
    GDrawSetUserData(dw,kf->second_fv);
    FVCopyInnards(kf->second_fv,&pos,infoh,fvorig,dw,kf->def_layer,(struct fvcontainer *) kf);

    kf->sf->display_size = ps;

    GGadgetGetSize(kf->second_fv->vsb,&sbsize);
    gsize.x = gsize.y = 0;
    gsize.width = pos.width + sbsize.width;
    gsize.height = pos.y+pos.height;
    GGadgetSetDesiredSize(drawable,NULL,&gsize);
}