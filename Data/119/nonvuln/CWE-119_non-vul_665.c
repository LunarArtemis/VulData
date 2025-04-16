static void FVMenuDisplaySubs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( fv->cur_subtable!=0 ) {
	fv->cur_subtable = NULL;
    } else {
	SplineFont *sf = fv->b.sf;
	OTLookup *otf;
	struct lookup_subtable *sub;
	int cnt, k;
	char **names = NULL;
	if ( sf->cidmaster ) sf=sf->cidmaster;
	for ( k=0; k<2; ++k ) {
	    cnt = 0;
	    for ( otf = sf->gsub_lookups; otf!=NULL; otf=otf->next ) {
		if ( otf->lookup_type==gsub_single ) {
		    for ( sub=otf->subtables; sub!=NULL; sub=sub->next ) {
			if ( names )
			    names[cnt] = sub->subtable_name;
			++cnt;
		    }
		}
	    }
	    if ( cnt==0 )
	break;
	    if ( names==NULL )
		names = malloc((cnt+3) * sizeof(char *));
	    else {
		names[cnt++] = "-";
		names[cnt++] = _("New Lookup Subtable...");
		names[cnt] = NULL;
	    }
	}
	sub = NULL;
	if ( names!=NULL ) {
	    int ret = gwwv_choose(_("Display Substitution..."), (const char **) names, cnt, 0,
		    _("Pick a substitution to display in the window."));
	    if ( ret!=-1 )
		sub = SFFindLookupSubtable(sf,names[ret]);
	    free(names);
	    if ( ret==-1 )
return;
	}
	if ( sub==NULL )
	    sub = SFNewLookupSubtableOfType(sf,gsub_single,NULL,fv->b.active_layer);
	if ( sub!=NULL )
	    fv->cur_subtable = sub;
    }
    GDrawRequestExpose(fv->v,NULL,false);
}