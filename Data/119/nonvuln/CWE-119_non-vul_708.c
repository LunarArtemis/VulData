static void FVMenuSetColor(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    Color col = (Color) (intpt) (mi->ti.userdata);
    if ( (intpt) mi->ti.userdata == (intpt) -10 ) {
	struct hslrgb retcol, font_cols[6];
	retcol = GWidgetColor(_("Pick a color"),NULL,SFFontCols(fv->b.sf,font_cols));
	if ( !retcol.rgb )
return;
	col = (((int) rint(255.*retcol.r))<<16 ) |
		    (((int) rint(255.*retcol.g))<<8 ) |
		    (((int) rint(255.*retcol.b)) );
    }
    FVSetColor(fv,col);
}