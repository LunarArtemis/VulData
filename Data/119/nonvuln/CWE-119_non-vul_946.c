static GTextInfo *GListFGetItem(GGadget *g,int32 pos) {
    GListField *gl = (GListField *) g;
    if ( pos<0 || pos>=gl->ltot )
return( NULL );

return(gl->ti[pos]);
}