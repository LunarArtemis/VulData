static void FVMenuConvert2CID(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *cidmaster = fv->b.cidmaster;
    struct cidmap *cidmap;

    if ( cidmaster!=NULL )
return;
    SFFindNearTop(fv->b.sf);
    cidmap = AskUserForCIDMap();
    if ( cidmap==NULL )
return;
    MakeCIDMaster(fv->b.sf,fv->b.map,false,NULL,cidmap);
    SFRestoreNearTop(fv->b.sf);
}