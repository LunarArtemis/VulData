static SplineFont *FontOfFilename(const char *filename) {
    char buffer[1025];
    FontView *fv;

    GFileGetAbsoluteName((char *) filename,buffer,sizeof(buffer));
    for ( fv=fv_list; fv!=NULL ; fv=(FontView *) (fv->b.next) ) {
	if ( fv->b.sf->filename!=NULL && strcmp(fv->b.sf->filename,buffer)==0 )
return( fv->b.sf );
	else if ( fv->b.sf->origname!=NULL && strcmp(fv->b.sf->origname,buffer)==0 )
return( fv->b.sf );
    }
return( NULL );
}