static int FeatureTrans(FontView *fv, int enc) {
    SplineChar *sc;
    PST *pst;
    char *pt;
    int gid;

    if ( enc<0 || enc>=fv->b.map->enccount || (gid = fv->b.map->map[enc])==-1 )
return( -1 );
    if ( fv->cur_subtable==NULL )
return( gid );

    sc = fv->b.sf->glyphs[gid];
    if ( sc==NULL )
return( -1 );
    for ( pst = sc->possub; pst!=NULL; pst=pst->next ) {
	if (( pst->type == pst_substitution || pst->type == pst_alternate ) &&
		pst->subtable == fv->cur_subtable )
    break;
    }
    if ( pst==NULL )
return( -1 );
    pt = strchr(pst->u.subs.variant,' ');
    if ( pt!=NULL )
	*pt = '\0';
    gid = SFFindExistingSlot(fv->b.sf, -1, pst->u.subs.variant );
    if ( pt!=NULL )
	*pt = ' ';
return( gid );
}