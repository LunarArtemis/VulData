static void hglistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
        if ( mi->mid==MID_BuildSyllables || mi->mid==MID_ModifyComposition )
	    mi->ti.disabled = fv->b.sf->rules==NULL;
    }
}