static void FVMenuWSize(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int h,v;
    extern int default_fv_col_count, default_fv_row_count;

    if ( mi->mid == MID_32x8 ) {
	h = 32; v=8;
    } else if ( mi->mid == MID_16x4 ) {
	h = 16; v=4;
    } else {
	h = 8; v=2;
    }
    GDrawResize(fv->gw,
	    h*fv->cbw+1+GDrawPointsToPixels(fv->gw,_GScrollBar_Width),
	    v*fv->cbh+1+fv->mbh+fv->infoh);
    fv->b.sf->desired_col_cnt = default_fv_col_count = h;
    fv->b.sf->desired_row_cnt = default_fv_row_count = v;

    SavePrefs(true);
}