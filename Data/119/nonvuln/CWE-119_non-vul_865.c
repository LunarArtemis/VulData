void FontViewRemove(FontView *fv) {
    if ( fv_list==fv )
	fv_list = (FontView *) (fv->b.next);
    else {
	FontView *n;
	for ( n=fv_list; n->b.next!=&fv->b; n=(FontView *) (n->b.next) );
	n->b.next = fv->b.next;
    }
    FontViewFree(&fv->b);
}