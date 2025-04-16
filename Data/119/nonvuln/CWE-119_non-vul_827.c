static void FVMenuPOV(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    struct pov_data pov_data;
    if ( FVAnyCharSelected(fv)==-1 || fv->b.sf->onlybitmaps )
return;
    if ( PointOfViewDlg(&pov_data,fv->b.sf,false)==-1 )
return;
    FVPointOfView((FontViewBase *) fv,&pov_data);
}