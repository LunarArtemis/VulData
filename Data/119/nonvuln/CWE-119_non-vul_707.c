static void FVMenuInsertFont(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *cidmaster = fv->b.cidmaster;
    SplineFont *new;
    struct cidmap *map;
    char *filename;
    extern NameList *force_names_when_opening;

    if ( cidmaster==NULL || cidmaster->subfontcnt>=255 )	/* Open type allows 1 byte to specify the fdselect */
return;

    filename = GetPostScriptFontName(NULL,false);
    if ( filename==NULL )
return;
    new = LoadSplineFont(filename,0);
    free(filename);
    if ( new==NULL )
return;
    if ( new->fv == &fv->b )		/* Already part of us */
return;
    if ( new->fv != NULL ) {
	if ( ((FontView *) (new->fv))->gw!=NULL )
	    GDrawRaise( ((FontView *) (new->fv))->gw);
	ff_post_error(_("Please close font"),_("Please close %s before inserting it into a CID font"),new->origname);
return;
    }
    EncMapFree(new->map);
    if ( force_names_when_opening!=NULL )
	SFRenameGlyphsToNamelist(new,force_names_when_opening );

    map = FindCidMap(cidmaster->cidregistry,cidmaster->ordering,cidmaster->supplement,cidmaster);
    SFEncodeToMap(new,map);
    if ( !PSDictHasEntry(new->private,"lenIV"))
	PSDictChangeEntry(new->private,"lenIV","1");		/* It's 4 by default, in CIDs the convention seems to be 1 */
    new->display_antialias = fv->b.sf->display_antialias;
    new->display_bbsized = fv->b.sf->display_bbsized;
    new->display_size = fv->b.sf->display_size;
    FVInsertInCID((FontViewBase *) fv,new);
    CIDMasterAsDes(new);
}