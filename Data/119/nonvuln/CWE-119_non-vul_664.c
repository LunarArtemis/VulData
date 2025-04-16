void FVSetUIToMatch(FontView *destfv,FontView *srcfv) {
    extern int use_freetype_to_rasterize_fv;

    if ( destfv->filled==NULL || srcfv->filled==NULL )
return;
    if ( destfv->magnify!=srcfv->magnify ||
	    destfv->user_requested_magnify!=srcfv->user_requested_magnify ||
	    destfv->bbsized!=srcfv->bbsized ||
	    destfv->antialias!=srcfv->antialias ||
	    destfv->filled->pixelsize != srcfv->filled->pixelsize ) {
	BDFFont *new, *old;
	destfv->magnify = srcfv->magnify;
	destfv->user_requested_magnify = srcfv->user_requested_magnify;
	destfv->bbsized = srcfv->bbsized;
	destfv->antialias = srcfv->antialias;
	old = destfv->filled;
	new = SplineFontPieceMeal(destfv->b.sf,destfv->b.active_layer,srcfv->filled->pixelsize,72,
	    (destfv->antialias?pf_antialias:0)|(destfv->bbsized?pf_bbsized:0)|
		(use_freetype_to_rasterize_fv && !destfv->b.sf->strokedfont && !destfv->b.sf->multilayer?pf_ft_nohints:0),
	    NULL);
	destfv->filled = new;
	FVChangeDisplayFont(destfv,new);
	BDFFontFree(old);
    }
}