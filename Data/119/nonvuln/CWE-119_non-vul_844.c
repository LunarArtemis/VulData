static void FVMenuFlattenByCMap(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *cidmaster = fv->b.cidmaster;
    char *cmapname;

    if ( cidmaster==NULL )
return;
    cmapname = gwwv_open_filename(_("Find an adobe CMap file..."),NULL,NULL,CMapFilter);
    if ( cmapname==NULL )
return;
    SFFindNearTop(fv->b.sf);
    SFFlattenByCMap(&cidmaster,cmapname);
    SFRestoreNearTop(fv->b.sf);
    free(cmapname);
}