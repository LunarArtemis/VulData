static void FVMenuCopyWidth(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( FVAnyCharSelected(fv)==-1 )
return;
    if ( mi->mid==MID_CopyVWidth && !fv->b.sf->hasvmetrics )
return;
    FVCopyWidth((FontViewBase *) fv,
		   mi->mid==MID_CopyWidth?ut_width:
		   mi->mid==MID_CopyVWidth?ut_vwidth:
		   mi->mid==MID_CopyLBearing?ut_lbearing:
					 ut_rbearing);
}