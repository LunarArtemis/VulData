static void cflistcheck(GWindow UNUSED(gw), struct gmenuitem *mi, GEvent *UNUSED(e)) {
    /*FontView *fv = (FontView *) GDrawGetUserData(gw);*/

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_AllFonts:
	    mi->ti.checked = !onlycopydisplayed;
	  break;
	  case MID_DisplayedFont:
	    mi->ti.checked = onlycopydisplayed;
	  break;
	  case MID_CharName:
	    mi->ti.checked = copymetadata;
	  break;
	  case MID_TTFInstr:
	    mi->ti.checked = copyttfinstr;
	  break;
	}
    }
}