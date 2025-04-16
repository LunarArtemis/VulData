static void FVMenuSize(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int dspsize = fv->filled->pixelsize;
    int changedmodifier = false;
    extern int use_freetype_to_rasterize_fv;

    fv->magnify = 1;
    fv->user_requested_magnify = -1;
    if ( mi->mid == MID_24 )
	default_fv_font_size = dspsize = 24;
    else if ( mi->mid == MID_36 )
	default_fv_font_size = dspsize = 36;
    else if ( mi->mid == MID_48 )
	default_fv_font_size = dspsize = 48;
    else if ( mi->mid == MID_72 )
	default_fv_font_size = dspsize = 72;
    else if ( mi->mid == MID_96 )
	default_fv_font_size = dspsize = 96;
    else if ( mi->mid == MID_128 )
	default_fv_font_size = dspsize = 128;
    else if ( mi->mid == MID_FitToBbox ) {
	default_fv_bbsized = fv->bbsized = !fv->bbsized;
	fv->b.sf->display_bbsized = fv->bbsized;
	changedmodifier = true;
    } else {
	default_fv_antialias = fv->antialias = !fv->antialias;
	fv->b.sf->display_antialias = fv->antialias;
	changedmodifier = true;
    }

    SavePrefs(true);
    if ( fv->filled!=fv->show || fv->filled->pixelsize != dspsize || changedmodifier ) {
	BDFFont *new, *old;
	old = fv->filled;
	new = SplineFontPieceMeal(fv->b.sf,fv->b.active_layer,dspsize,72,
	    (fv->antialias?pf_antialias:0)|(fv->bbsized?pf_bbsized:0)|
		(use_freetype_to_rasterize_fv && !fv->b.sf->strokedfont && !fv->b.sf->multilayer?pf_ft_nohints:0),
	    NULL);
	fv->filled = new;
	FVChangeDisplayFont(fv,new);
	BDFFontFree(old);
	fv->b.sf->display_size = -dspsize;
	if ( fv->b.cidmaster!=NULL ) {
	    int i;
	    for ( i=0; i<fv->b.cidmaster->subfontcnt; ++i )
		fv->b.cidmaster->subfonts[i]->display_size = -dspsize;
	}
    }
}