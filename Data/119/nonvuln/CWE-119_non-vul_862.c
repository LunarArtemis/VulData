static int IsBackupName(char *filename) {

    if ( filename==NULL )
return( false );
return( filename[strlen(filename)-1]=='~' );
}