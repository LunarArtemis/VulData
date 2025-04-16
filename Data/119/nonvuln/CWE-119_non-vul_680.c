static void FVMenuSetWidth(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( FVAnyCharSelected(fv)==-1 )
return;
    if ( mi->mid == MID_SetVWidth && !fv->b.sf->hasvmetrics )
return;
    FVSetWidth(fv,mi->mid==MID_SetWidth   ?wt_width:
		  mi->mid==MID_SetLBearing?wt_lbearing:
		  mi->mid==MID_SetRBearing?wt_rbearing:
		  mi->mid==MID_SetBearings?wt_bearings:
		  wt_vwidth);
}