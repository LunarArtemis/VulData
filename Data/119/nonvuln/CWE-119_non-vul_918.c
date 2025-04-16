static void FVMenuAddEncodingName(GWindow UNUSED(gw), struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    char *ret;
    Encoding *enc;

    /* Search the iconv database for the named encoding */
    ret = gwwv_ask_string(_("Add Encoding Name..."),NULL,_("Please provide the name of an encoding in the iconv database which you want in the menu."));
    if ( ret==NULL )
return;
    enc = FindOrMakeEncoding(ret);
    if ( enc==NULL )
	ff_post_error(_("Invalid Encoding"),_("Invalid Encoding"));
    free(ret);
}