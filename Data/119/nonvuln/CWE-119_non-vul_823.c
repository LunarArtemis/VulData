static void FVMenuSetExtremumBound(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    char buffer[40], *end, *ret;
    int val;

    sprintf( buffer, "%d", fv->b.sf->extrema_bound<=0 ?
	    (int) rint((fv->b.sf->ascent+fv->b.sf->descent)/100.0) :
	    fv->b.sf->extrema_bound );
    ret = gwwv_ask_string(_("Extremum bound..."),buffer,_("Adobe says that \"big\" splines should not have extrema.\nBut they don't define what big means.\nIf the distance between the spline's end-points is bigger than this value, then the spline is \"big\" to fontforge."));
    if ( ret==NULL )
return;
    val = (int) rint(strtod(ret,&end));
    if ( *end!='\0' )
	ff_post_error( _("Bad Number"),_("Bad Number") );
    else {
	fv->b.sf->extrema_bound = val;
	if ( !fv->b.sf->changed ) {
	    fv->b.sf->changed = true;
	    FVSetTitles(fv->b.sf);
	}
    }
    free(ret);
}