static void validlistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int anychars = FVAnyCharSelected(fv);

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_FindProblems:
	    mi->ti.disabled = anychars==-1;
	  break;
	  case MID_Validate:
	    mi->ti.disabled = fv->b.sf->strokedfont || fv->b.sf->multilayer;
	  break;
        }
    }
}