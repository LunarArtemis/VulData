static void FV_LayerChanged( FontView *fv ) {
    extern int use_freetype_to_rasterize_fv;
    BDFFont *new, *old;

    fv->magnify = 1;
    fv->user_requested_magnify = -1;

    old = fv->filled;
    new = SplineFontPieceMeal(fv->b.sf,fv->b.active_layer,fv->filled->pixelsize,72,
	(fv->antialias?pf_antialias:0)|(fv->bbsized?pf_bbsized:0)|
	    (use_freetype_to_rasterize_fv && !fv->b.sf->strokedfont && !fv->b.sf->multilayer?pf_ft_nohints:0),
	NULL);
    fv->filled = new;
    FVChangeDisplayFont(fv,new);
    fv->b.sf->display_size = -fv->filled->pixelsize;
    BDFFontFree(old);
}