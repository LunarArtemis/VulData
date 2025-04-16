static void FVMenuConvertByCMap(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *cidmaster = fv->b.cidmaster;
    char *cmapfilename;

    if ( cidmaster!=NULL )
return;
    cmapfilename = gwwv_open_filename(_("Find an adobe CMap file..."),NULL,NULL,CMapFilter);
    if ( cmapfilename==NULL )
return;
    MakeCIDMaster(fv->b.sf,fv->b.map,true,cmapfilename,NULL);
    free(cmapfilename);
}