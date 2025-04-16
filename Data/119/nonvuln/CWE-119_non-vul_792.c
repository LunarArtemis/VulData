static void FVMenuMagnify(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int magnify = fv->user_requested_magnify!=-1 ? fv->user_requested_magnify : fv->magnify;
    char def[20], *end, *ret;
    int val;
    BDFFont *show = fv->show;

    sprintf( def, "%d", magnify );
    ret = gwwv_ask_string(_("Bitmap Magnification..."),def,_("Please specify a bitmap magnification factor."));
    if ( ret==NULL )
return;
    val = strtol(ret,&end,10);
    if ( val<1 || val>5 || *end!='\0' )
	ff_post_error( _("Bad Number"),_("Bad Number") );
    else {
	fv->user_requested_magnify = val;
	fv->show = fv->filled;
	fv->b.active_bitmap = NULL;
	FVChangeDisplayFont(fv,show);
    }
    free(ret);
}