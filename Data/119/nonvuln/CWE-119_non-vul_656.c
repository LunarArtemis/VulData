static void FVSetColor(FontView *fv, uint32 col) {
    int i;

    for ( i=0; i<fv->b.map->enccount; ++i ) if ( fv->b.selected[i] ) {
	SplineChar *sc = SFMakeChar(fv->b.sf,fv->b.map,i);
	sc->color = col;
    }
    GDrawRequestExpose(fv->v,NULL,false);
}