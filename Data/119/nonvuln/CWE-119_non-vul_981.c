static int32 GListFGetFirst(GGadget *g) {
    int i;
    GListField *gl = (GListField *) g;

    for ( i=0; i<gl->ltot; ++i )
	if ( gl->ti[i]->selected )
return( i );

return( -1 );
}