static unichar_t **GListField_NameCompletion(GGadget *t,int from_tab) {
    const unichar_t *spt; unichar_t **ret;
    GTextInfo **ti;
    int32 len;
    int i, cnt, doit, match_len;

    spt = _GGadgetGetTitle(t);
    if ( spt==NULL )
return( NULL );

    match_len = u_strlen(spt);
    ti = GGadgetGetList(t,&len);
    ret = NULL;
    for ( doit=0; doit<2; ++doit ) {
	cnt=0;
	for ( i=0; i<len; ++i ) {
	    if ( ti[i]->text && u_strncmp(ti[i]->text,spt,match_len)==0 ) {
		if ( doit )
		    ret[cnt] = u_copy(ti[i]->text);
		++cnt;
	    }
	}
	if ( doit )
	    ret[cnt] = NULL;
	else if ( cnt==0 )
return( NULL );
	else
	    ret = malloc((cnt+1)*sizeof(unichar_t *));
    }
return( ret );
}