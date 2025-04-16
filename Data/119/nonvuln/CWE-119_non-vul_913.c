static int FVSelCount(FontView *fv) {
    int i, cnt=0;

    for ( i=0; i<fv->b.map->enccount; ++i )
	if ( fv->b.selected[i] ) ++cnt;
    if ( cnt>10 ) {
	char *buts[3];
	buts[0] = _("_OK");
	buts[1] = _("_Cancel");
	buts[2] = NULL;
	if ( gwwv_ask(_("Many Windows"),(const char **) buts,0,1,_("This involves opening more than 10 windows.\nIs that really what you want?"))==1 )
return( false );
    }
return( true );
}