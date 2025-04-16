static int AskChanged(SplineFont *sf) {
    int ret;
    char *buts[4];
    char *filename, *fontname;

    if ( sf->cidmaster!=NULL )
	sf = sf->cidmaster;

    filename = sf->filename;
    fontname = sf->fontname;

    if ( filename==NULL && sf->origname!=NULL &&
	    sf->onlybitmaps && sf->bitmaps!=NULL && sf->bitmaps->next==NULL )
	filename = sf->origname;
    if ( filename==NULL ) filename = "untitled.sfd";
    filename = GFileNameTail(filename);
    buts[0] = _("_Save");
    buts[1] = _("_Don't Save");
    buts[2] = _("_Cancel");
    buts[3] = NULL;
    ret = gwwv_ask( _("Font changed"),(const char **) buts,0,2,_("Font %1$.40s in file %2$.40s has been changed.\nDo you want to save it?"),fontname,filename);
return( ret );
}