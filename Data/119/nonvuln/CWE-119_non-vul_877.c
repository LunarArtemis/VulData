static void FVMenuHistograms(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SFHistogram(fv->b.sf, fv->b.active_layer, NULL,
			FVAnyCharSelected(fv)!=-1?fv->b.selected:NULL,
			fv->b.map,
			mi->mid==MID_HStemHist ? hist_hstem :
			mi->mid==MID_VStemHist ? hist_vstem :
				hist_blues);
}