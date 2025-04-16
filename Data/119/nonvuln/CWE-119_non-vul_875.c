static void FVMenuNameGlyphs(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    /* Read a file containing a list of names, and add an unencoded glyph for */
    /*  each name */
    char buffer[33];
    char *ret = gwwv_open_filename(_("Load glyph names"),NULL, "*",NULL);
    char *temp, *pt;
    FILE *file;
    int ch;
    SplineChar *sc;
    FontView *fvs;

    if ( ret==NULL )
return;				/* Cancelled */
    temp = utf82def_copy(ret);

    file = fopen( temp,"r");
    if ( file==NULL ) {
	ff_post_error(_("No such file"),_("Could not read %s"), ret );
	free(ret); free(temp);
return;
    }
    pt = buffer;
    for (;;) {
	ch = getc(file);
	if ( ch!=EOF && !isspace(ch)) {
	    if ( pt<buffer+sizeof(buffer)-1 )
		*pt++ = ch;
	} else {
	    if ( pt!=buffer ) {
		*pt = '\0';
		sc = NULL;
		for ( fvs=(FontView *) (fv->b.sf->fv); fvs!=NULL; fvs=(FontView *) (fvs->b.nextsame) ) {
		    EncMap *map = fvs->b.map;
		    if ( map->enccount+1>=map->encmax )
			map->map = realloc(map->map,(map->encmax += 20)*sizeof(int));
		    map->map[map->enccount] = -1;
		    fvs->b.selected = realloc(fvs->b.selected,(map->enccount+1));
		    memset(fvs->b.selected+map->enccount,0,1);
		    ++map->enccount;
		    if ( sc==NULL ) {
			sc = SFMakeChar(fv->b.sf,map,map->enccount-1);
			free(sc->name);
			sc->name = copy(buffer);
			sc->comment = copy(".");	/* Mark as something for sfd file */
		    }
		    map->map[map->enccount-1] = sc->orig_pos;
		    map->backmap[sc->orig_pos] = map->enccount-1;
		}
		pt = buffer;
	    }
	    if ( ch==EOF )
    break;
	}
    }
    fclose(file);
    free(ret); free(temp);
    FontViewReformatAll(fv->b.sf);
}