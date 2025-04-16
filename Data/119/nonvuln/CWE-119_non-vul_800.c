static void infolistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int anychars = FVAnyCharSelected(fv);

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_StrikeInfo:
	    mi->ti.disabled = fv->b.sf->bitmaps==NULL;
	  break;
	  case MID_MassRename:
	    mi->ti.disabled = anychars==-1;
	  break;
	  case MID_SetColor:
	    mi->ti.disabled = anychars==-1;
	  break;
	}
    }
}