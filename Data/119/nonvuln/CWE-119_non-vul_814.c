static void trlistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int anychars = FVAnyCharSelected(fv);

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_Transform:
	    mi->ti.disabled = anychars==-1;
	  break;
	  case MID_NLTransform: case MID_POV:
	    mi->ti.disabled = anychars==-1 || fv->b.sf->onlybitmaps;
	  break;
	}
    }
}