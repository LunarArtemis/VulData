static SplineChar *FVFindACharInDisplay(FontView *fv) {
    int start, end, enc, gid;
    EncMap *map = fv->b.map;
    SplineFont *sf = fv->b.sf;
    SplineChar *sc;

    start = fv->rowoff*fv->colcnt;
    end = start + fv->rowcnt*fv->colcnt;
    for ( enc = start; enc<end && enc<map->enccount; ++enc ) {
	if ( (gid=map->map[enc])!=-1 && (sc=sf->glyphs[gid])!=NULL )
return( sc );
    }
return( NULL );
}