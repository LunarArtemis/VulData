static void mtlistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int anychars = FVAnyCharSelected(fv);

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_Center: case MID_Thirds: case MID_SetWidth:
	  case MID_SetLBearing: case MID_SetRBearing: case MID_SetBearings:
	    mi->ti.disabled = anychars==-1;
	  break;
	  case MID_SetVWidth:
	    mi->ti.disabled = anychars==-1 || !fv->b.sf->hasvmetrics;
	  break;
	  case MID_VKernByClass:
	  case MID_VKernFromH:
	  case MID_RmVKern:
	    mi->ti.disabled = !fv->b.sf->hasvmetrics;
	  break;
	}
    }
}