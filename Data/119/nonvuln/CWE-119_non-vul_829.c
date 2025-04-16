static void FVFlattenAllBitmapSelections(FontView *fv) {
    BDFFont *bdf;
    int i;

    for ( bdf = fv->b.sf->bitmaps; bdf!=NULL; bdf=bdf->next ) {
	for ( i=0; i<bdf->glyphcnt; ++i )
	    if ( bdf->glyphs[i]!=NULL && bdf->glyphs[i]->selection!=NULL )
		BCFlattenFloat(bdf->glyphs[i]);
    }
}