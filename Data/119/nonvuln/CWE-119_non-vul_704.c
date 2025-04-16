static void FVMenuChangeSupplement(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    SplineFont *cidmaster = fv->b.cidmaster;
    struct cidmap *cidmap;
    char buffer[20];
    char *ret, *end;
    int supple;

    if ( cidmaster==NULL )
return;
    sprintf(buffer,"%d",cidmaster->supplement);
    ret = gwwv_ask_string(_("Change Supplement..."),buffer,_("Please specify a new supplement for %.20s-%.20s"),
	    cidmaster->cidregistry,cidmaster->ordering);
    if ( ret==NULL )
return;
    supple = strtol(ret,&end,10);
    if ( *end!='\0' || supple<=0 ) {
	free(ret);
	ff_post_error( _("Bad Number"),_("Bad Number") );
return;
    }
    free(ret);
    if ( supple!=cidmaster->supplement ) {
	    /* this will make noises if it can't find an appropriate cidmap */
	cidmap = FindCidMap(cidmaster->cidregistry,cidmaster->ordering,supple,cidmaster);
	cidmaster->supplement = supple;
	FontViewSetTitle(fv);
    }
}