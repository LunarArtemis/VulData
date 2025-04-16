static FontView *FontView_Create(SplineFont *sf, int hide) {
    FontView *fv = (FontView *) __FontViewCreate(sf);
    GRect pos;
    GWindow gw;
    GWindowAttrs wattrs;
    GGadgetData gd;
    GRect gsize;
    static GWindow icon = NULL;
    static int nexty=0;
    GRect size;

    FontViewInit();
    if ( icon==NULL ) {
#ifdef BIGICONS
	icon = GDrawCreateBitmap(NULL,fontview_width,fontview_height,fontview_bits);
#else
	icon = GDrawCreateBitmap(NULL,fontview2_width,fontview2_height,fontview2_bits);
#endif
    }

    GDrawGetSize(GDrawGetRoot(NULL),&size);

    memset(&wattrs,0,sizeof(wattrs));
    wattrs.mask = wam_events|wam_cursor|wam_icon;
    wattrs.event_masks = ~(1<<et_charup);
    wattrs.cursor = ct_pointer;
    wattrs.icon = icon;
    pos.width = sf->desired_col_cnt*fv->cbw+1;
    pos.height = sf->desired_row_cnt*fv->cbh+1;
    pos.x = size.width-pos.width-30; pos.y = nexty;
    nexty += 2*fv->cbh+50;
    if ( nexty+pos.height > size.height )
	nexty = 0;
    fv->gw = gw = GDrawCreateTopWindow(NULL,&pos,fv_e_h,fv,&wattrs);
    FontViewSetTitle(fv);
    GDrawSetWindowTypeName(fv->gw, "FontView");

    if ( !fv_fs_init ) {
	GResEditFind( fontview_re, "FontView.");
	view_bgcol = GResourceFindColor("View.Background",GDrawGetDefaultBackground(NULL));
	fv_fs_init = true;
    }

    memset(&gd,0,sizeof(gd));
    gd.flags = gg_visible | gg_enabled;
    helplist[0].invoke = FVMenuContextualHelp;
#ifndef _NO_PYTHON
    if ( fvpy_menu!=NULL )
	mblist[3].ti.disabled = false;
    mblist[3].sub = fvpy_menu;
#define CALLBACKS_INDEX 4 /* FIXME: There has to be a better way than this. */
#else
#define CALLBACKS_INDEX 3 /* FIXME: There has to be a better way than this. */
#endif		/* _NO_PYTHON */
#ifdef NATIVE_CALLBACKS
    if ( fv_menu!=NULL )
       mblist[CALLBACKS_INDEX].ti.disabled = false;
    mblist[CALLBACKS_INDEX].sub = fv_menu;
#endif      /* NATIVE_CALLBACKS */
    gd.u.menu2 = mblist;
    fv->mb = GMenu2BarCreate( gw, &gd, NULL);
    GGadgetGetSize(fv->mb,&gsize);
    fv->mbh = gsize.height;
    fv->infoh = 1+GDrawPointsToPixels(NULL,fv_fontsize);

    pos.x = 0; pos.y = fv->mbh+fv->infoh;
    FVCreateInnards(fv,&pos);

    if ( !hide ) {
	GDrawSetVisible(gw,true);
	FontViewOpenKids(fv);
    }
return( fv );
}