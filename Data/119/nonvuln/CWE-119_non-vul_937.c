void FVDeselectAll(FontView *fv) {
    int i;

    for ( i=0; i<fv->b.map->enccount; ++i ) {
	if ( fv->b.selected[i] ) {
	    fv->b.selected[i] = false;
	    FVToggleCharSelected(fv,i);
	}
    }
    fv->sel_index = 0;
}