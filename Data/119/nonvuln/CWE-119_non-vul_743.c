static void FVMenuChangeLayer(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    fv->b.active_layer = mi->mid;
    fv->b.sf->display_layer = mi->mid;
    FV_LayerChanged(fv);
}