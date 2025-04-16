int _FVMenuGenerate(FontView *fv,int family) {
    FVFlattenAllBitmapSelections(fv);
return( SFGenerateFont(fv->b.sf,fv->b.active_layer,family,fv->b.normal==NULL?fv->b.map:fv->b.normal) );
}