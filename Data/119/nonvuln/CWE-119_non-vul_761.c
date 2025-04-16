static int FVAllSelected(FontView *fv) {
    int i, any = false;
    /* Is everything real selected? */

    for ( i=0; i<fv->b.sf->glyphcnt; ++i ) if ( SCWorthOutputting(fv->b.sf->glyphs[i])) {
	if ( !fv->b.selected[fv->b.map->backmap[i]] )
return( false );
	any = true;
    }
return( any );
}