static void htlistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int anychars = FVAnyCharSelected(fv);
    int multilayer = fv->b.sf->multilayer;

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_AutoHint:
	    mi->ti.disabled = anychars==-1 || multilayer;
	  break;
	  case MID_HintSubsPt:
	    mi->ti.disabled = fv->b.sf->layers[fv->b.active_layer].order2 || anychars==-1 || multilayer;
	    if ( fv->b.sf->mm!=NULL && fv->b.sf->mm->apple )
		mi->ti.disabled = true;
	  break;
	  case MID_AutoCounter: case MID_DontAutoHint:
	    mi->ti.disabled = fv->b.sf->layers[fv->b.active_layer].order2 || anychars==-1 || multilayer;
	  break;
	  case MID_AutoInstr: case MID_EditInstructions: case MID_Deltas:
	    mi->ti.disabled = !fv->b.sf->layers[fv->b.active_layer].order2 || anychars==-1 || multilayer;
	  break;
	  case MID_RmInstrTables:
	    mi->ti.disabled = fv->b.sf->ttf_tables==NULL;
	  break;
	  case MID_Editfpgm: case MID_Editprep: case MID_Editcvt: case MID_Editmaxp:
	    mi->ti.disabled = !fv->b.sf->layers[fv->b.active_layer].order2 || multilayer;
	  break;
	  case MID_ClearHints: case MID_ClearWidthMD: case MID_ClearInstrs:
	    mi->ti.disabled = anychars==-1;
	  break;
	}
    }
}