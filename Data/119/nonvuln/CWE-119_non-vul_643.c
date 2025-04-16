static void FVMenuLoadNamelist(GWindow UNUSED(gw), struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    /* Read in a name list and copy it into the prefs dir so that we'll find */
    /*  it in the future */
    /* Be prepared to update what we've already got if names match */
    char buffer[1025];
    char *ret = gwwv_open_filename(_("Load Namelist"),NULL,
	    "*.nam",NULL);
    char *temp, *pt;
    char *buts[3];
    FILE *old, *new;
    int ch, ans;
    NameList *nl;

    if ( ret==NULL )
return;				/* Cancelled */
    temp = utf82def_copy(ret);
    pt = strrchr(temp,'/');
    if ( pt==NULL )
	pt = temp;
    else
	++pt;
    snprintf(buffer,sizeof(buffer),"%s/%s", getFontForgeUserDir(Config), pt);
    if ( access(buffer,F_OK)==0 ) {
	buts[0] = _("_Replace");
	buts[1] = _("_Cancel");
	buts[2] = NULL;
	ans = gwwv_ask( _("Replace"),(const char **) buts,0,1,_("A name list with this name already exists. Replace it?"));
	if ( ans==1 ) {
	    free(temp);
	    free(ret);
return;
	}
    }

    old = fopen( temp,"r");
    if ( old==NULL ) {
	ff_post_error(_("No such file"),_("Could not read %s"), ret );
	free(ret); free(temp);
return;
    }
    if ( (nl = LoadNamelist(temp))==NULL ) {
	ff_post_error(_("Bad namelist file"),_("Could not parse %s"), ret );
	free(ret); free(temp);
        fclose(old);
return;
    }
    free(ret); free(temp);
    if ( nl->uses_unicode ) {
	if ( nl->a_utf8_name!=NULL )
	    ff_post_notice(_("Non-ASCII glyphnames"),_("This namelist contains at least one non-ASCII glyph name, namely: %s"), nl->a_utf8_name );
	else
	    ff_post_notice(_("Non-ASCII glyphnames"),_("This namelist is based on a namelist which contains non-ASCII glyph names"));
    }

    new = fopen( buffer,"w");
    if ( new==NULL ) {
	ff_post_error(_("Create failed"),_("Could not write %s"), buffer );
        fclose(old);
return;
    }

    while ( (ch=getc(old))!=EOF )
	putc(ch,new);
    fclose(old);
    fclose(new);
}