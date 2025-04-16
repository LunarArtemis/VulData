static int FVSelectByName(FontView *fv, char *ret, int merge) {
    int j, gid, doit;
    char *end;
    SplineChar *sc;
    EncMap *map = fv->b.map;
    SplineFont *sf = fv->b.sf;
    struct altuni *alt;

    if ( !merge )
	FVDeselectAll(fv);
    if (( *ret=='0' && ( ret[1]=='x' || ret[1]=='X' )) ||
	    ((*ret=='u' || *ret=='U') && ret[1]=='+' )) {
	int uni = (int) strtol(ret+2,&end,16);
	int vs= -2;
	if ( *end=='.' ) {
	    ++end;
	    if (( *end=='0' && ( end[1]=='x' || end[1]=='X' )) ||
		    ((*end=='u' || *end=='U') && end[1]=='+' ))
		end += 2;
	    vs = (int) strtoul(end,&end,16);
	}
	if ( *end!='\0' || uni<0 || uni>=0x110000 ) {
	    ff_post_error( _("Bad Number"),_("Bad Number") );
return( false );
	}
	for ( j=0; j<map->enccount; ++j ) if ( (gid=map->map[j])!=-1 && (sc=sf->glyphs[gid])!=NULL ) {
	    if ( vs==-2 ) {
		for ( alt=sc->altuni; alt!=NULL && (alt->unienc!=uni || alt->fid!=0); alt=alt->next );
	    } else {
		for ( alt=sc->altuni; alt!=NULL && (alt->unienc!=uni || alt->vs!=vs || alt->fid!=0); alt=alt->next );
	    }
	    doit = (sc->unicodeenc == uni && vs<0) || alt!=NULL;
	    fv->b.selected[j] = mergefunc[ merge + (fv->b.selected[j]?2:0) + doit ];
	} else if ( merge==mt_set )
	    fv->b.selected[j] = false;
    } else {
	for ( j=0; j<map->enccount; ++j ) if ( (gid=map->map[j])!=-1 && (sc=sf->glyphs[gid])!=NULL ) {
	    doit = WildMatch(ret,sc->name,false);
	    fv->b.selected[j] = mergefunc[ merge + (fv->b.selected[j]?2:0) + doit ];
	} else if ( merge==mt_set )
	    fv->b.selected[j] = false;
    }
    GDrawRequestExpose(fv->v,NULL,false);
    fv->sel_index = 1;
return( true );
}