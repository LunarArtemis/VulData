static void *ddgencharlist(void *_fv,int32 *len) {
    int i,j,cnt, gid;
    FontView *fv = (FontView *) _fv;
    SplineFont *sf = fv->b.sf;
    EncMap *map = fv->b.map;
    char *data;

    for ( i=cnt=0; i<map->enccount; ++i ) if ( fv->b.selected[i] && (gid=map->map[i])!=-1 && sf->glyphs[gid]!=NULL )
	cnt += strlen(sf->glyphs[gid]->name)+1;
    data = malloc(cnt+1); data[0] = '\0';
    for ( cnt=0, j=1 ; j<=fv->sel_index; ++j ) {
	for ( i=cnt=0; i<map->enccount; ++i )
	    if ( fv->b.selected[i] && (gid=map->map[i])!=-1 && sf->glyphs[gid]!=NULL ) {
		strcpy(data+cnt,sf->glyphs[gid]->name);
		cnt += strlen(sf->glyphs[gid]->name);
		strcpy(data+cnt++," ");
	    }
    }
    if ( cnt>0 )
	data[--cnt] = '\0';
    *len = cnt;
return( data );
}