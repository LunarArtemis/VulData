void FVMarkHintsOutOfDate(SplineChar *sc) {
    int i, j;
    int pos;
    FontView *fv;

    if ( sc->parent->onlybitmaps || sc->parent->multilayer || sc->parent->strokedfont )
return;
    for ( fv = (FontView *) (sc->parent->fv); fv!=NULL; fv=(FontView *) (fv->b.nextsame) ) {
	if ( fv->b.sf!=sc->parent )		/* Can happen in CID fonts if char's parent is not currently active */
    continue;
	if ( sc->layers[fv->b.active_layer].order2 )
    continue;
	if ( fv->v==NULL || fv->colcnt==0 )	/* Can happen in scripts */
    continue;
	for ( pos=0; pos<fv->b.map->enccount; ++pos ) if ( fv->b.map->map[pos]==sc->orig_pos ) {
	    i = pos / fv->colcnt;
	    j = pos - i*fv->colcnt;
	    i -= fv->rowoff;
 /* Normally we should be checking against fv->rowcnt (rather than <=rowcnt) */
 /*  but every now and then the WM forces us to use a window size which doesn't */
 /*  fit our expectations (maximized view) and we must be prepared for half */
 /*  lines */
	    if ( i>=0 && i<=fv->rowcnt ) {
		GRect r;
		r.x = j*fv->cbw+1; r.width = fv->cbw-1;
		r.y = i*fv->cbh+1; r.height = fv->lab_height-1;
		GDrawDrawLine(fv->v,r.x,r.y,r.x,r.y+r.height-1,fvhintingneededcol);
		GDrawDrawLine(fv->v,r.x+1,r.y,r.x+1,r.y+r.height-1,fvhintingneededcol);
		GDrawDrawLine(fv->v,r.x+r.width-1,r.y,r.x+r.width-1,r.y+r.height-1,fvhintingneededcol);
		GDrawDrawLine(fv->v,r.x+r.width-2,r.y,r.x+r.width-2,r.y+r.height-1,fvhintingneededcol);
	    }
	}
    }
}