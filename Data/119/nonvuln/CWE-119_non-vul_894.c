static void FVInvertSelection(FontView *fv) {
    int i;

    for ( i=0; i<fv->b.map->enccount; ++i ) {
	fv->b.selected[i] = !fv->b.selected[i];
	FVToggleCharSelected(fv,i);
    }
    fv->sel_index = 1;
}