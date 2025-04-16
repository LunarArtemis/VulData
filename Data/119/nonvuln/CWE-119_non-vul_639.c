static void FontViewSetTitles(SplineFont *sf) {
    FontView *fv;

    for ( fv = (FontView *) (sf->fv); fv!=NULL; fv=(FontView *) (fv->b.nextsame))
	FontViewSetTitle(fv);
}