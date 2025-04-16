static FontView *FontView_Append(FontView *fv) {
    /* Normally fontviews get added to the fv list when their windows are */
    /*  created. but we don't create any windows here, so... */
    FontView *test;

    if ( fv_list==NULL ) fv_list = fv;
    else {
	for ( test = fv_list; test->b.next!=NULL; test=(FontView *) test->b.next );
	test->b.next = (FontViewBase *) fv;
    }
return( fv );
}