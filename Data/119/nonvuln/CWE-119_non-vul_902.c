static void FVMenuCenter(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontViewBase *fv = (FontViewBase *) GDrawGetUserData(gw);
    FVMetricsCenter(fv,mi->mid==MID_Center);
}