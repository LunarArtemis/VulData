static int  FontIsActive(SplineFont *sf) {
    FontView *fv;

    for ( fv=fv_list; fv!=NULL; fv=(FontView *) (fv->b.next) )
	if ( fv->b.sf == sf )
return( true );

return( false );
}