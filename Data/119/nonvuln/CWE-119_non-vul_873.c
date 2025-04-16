static void FV_ShowFilled(FontView *fv) {

    fv->magnify = 1;
    fv->user_requested_magnify = 1;
    if ( fv->show!=fv->filled )
	FVChangeDisplayFont(fv,fv->filled);
    fv->b.sf->display_size = -fv->filled->pixelsize;
    fv->b.active_bitmap = NULL;
}