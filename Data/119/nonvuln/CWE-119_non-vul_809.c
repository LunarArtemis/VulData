static void FVCreateInnards(FontView *fv,GRect *pos) {
    GWindow gw = fv->gw;
    GWindowAttrs wattrs;
    GGadgetData gd;
    FontRequest rq;
    BDFFont *bdf;
    int as,ds,ld;
    extern int use_freetype_to_rasterize_fv;
    SplineFont *sf = fv->b.sf;

    fv->lab_height = FV_LAB_HEIGHT-13+GDrawPointsToPixels(NULL,fv_fontsize);

    memset(&gd,0,sizeof(gd));
    gd.pos.y = pos->y; gd.pos.height = pos->height;
    gd.pos.width = GDrawPointsToPixels(gw,_GScrollBar_Width);
    gd.pos.x = pos->width;
    gd.u.sbinit = NULL;
    gd.flags = gg_visible|gg_enabled|gg_pos_in_pixels|gg_sb_vert;
    gd.handle_controlevent = FVScroll;
    fv->vsb = GScrollBarCreate(gw,&gd,fv);


    memset(&wattrs,0,sizeof(wattrs));
    wattrs.mask = wam_events|wam_cursor|wam_backcol;
    wattrs.event_masks = ~(1<<et_charup);
    wattrs.cursor = ct_pointer;
    wattrs.background_color = view_bgcol;
    fv->v = GWidgetCreateSubWindow(gw,pos,v_e_h,fv,&wattrs);
    GDrawSetVisible(fv->v,true);
    GDrawSetWindowTypeName(fv->v, "FontView");

    fv->gic   = GDrawCreateInputContext(fv->v,gic_root|gic_orlesser);
    fv->gwgic = GDrawCreateInputContext(fv->gw,gic_root|gic_orlesser);
    GDrawSetGIC(fv->v,fv->gic,0,20);
    GDrawSetGIC(fv->gw,fv->gic,0,20);

    fv->fontset = calloc(_uni_fontmax,sizeof(GFont *));
    memset(&rq,0,sizeof(rq));
    rq.utf8_family_name = fv_fontnames;
    rq.point_size = fv_fontsize;
    rq.weight = 400;
    fv->fontset[0] = GDrawInstanciateFont(gw,&rq);
    GDrawSetFont(fv->v,fv->fontset[0]);
    GDrawWindowFontMetrics(fv->v,fv->fontset[0],&as,&ds,&ld);
    fv->lab_as = as;
    fv->showhmetrics = default_fv_showhmetrics;
    fv->showvmetrics = default_fv_showvmetrics && sf->hasvmetrics;
    bdf = SplineFontPieceMeal(fv->b.sf,fv->b.active_layer,sf->display_size<0?-sf->display_size:default_fv_font_size,72,
	    (fv->antialias?pf_antialias:0)|(fv->bbsized?pf_bbsized:0)|
		(use_freetype_to_rasterize_fv && !sf->strokedfont && !sf->multilayer?pf_ft_nohints:0),
	    NULL);
    fv->filled = bdf;
    if ( sf->display_size>0 ) {
	for ( bdf=sf->bitmaps; bdf!=NULL && bdf->pixelsize!=sf->display_size ;
		bdf=bdf->next );
	if ( bdf==NULL )
	    bdf = fv->filled;
    }
    if ( sf->onlybitmaps && bdf==fv->filled && sf->bitmaps!=NULL )
	bdf = sf->bitmaps;
    fv->cbw = -1;
    FVChangeDisplayFont(fv,bdf);
}