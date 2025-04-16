static int SFAnyChanged(SplineFont *sf) {
    if ( sf->mm!=NULL ) {
	MMSet *mm = sf->mm;
	int i;
	if ( mm->changed )
return( true );
	for ( i=0; i<mm->instance_count; ++i )
	    if ( sf->mm->instances[i]->changed )
return( true );
	/* Changes to the blended font aren't real (for adobe fonts) */
	if ( mm->apple && mm->normal->changed )
return( true );

return( false );
    } else
return( sf->changed );
}