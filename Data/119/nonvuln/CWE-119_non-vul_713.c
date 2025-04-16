static void FVMenuAddUnencoded(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    char *ret, *end;
    int cnt;

    ret = gwwv_ask_string(_("Add Encoding Slots..."),"1",fv->b.cidmaster?_("How many CID slots do you wish to add?"):_("How many unencoded glyph slots do you wish to add?"));
    if ( ret==NULL )
return;
    cnt = strtol(ret,&end,10);
    if ( *end!='\0' || cnt<=0 ) {
	free(ret);
	ff_post_error( _("Bad Number"),_("Bad Number") );
return;
    }
    free(ret);
    FVAddUnencoded((FontViewBase *) fv, cnt);
}