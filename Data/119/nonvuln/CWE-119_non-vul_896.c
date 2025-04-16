static int WildMatch(char *pattern, char *name,int ignorecase) {
    char *eop = pattern + strlen(pattern);

    if ( pattern==NULL )
return( true );

    name = SubMatch(pattern,eop,name,ignorecase);
    if ( name==NULL )
return( false );
    if ( *name=='\0' )
return( true );

return( false );
}