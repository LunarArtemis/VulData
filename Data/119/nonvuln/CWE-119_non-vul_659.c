static void FVDoTransform(FontView *fv) {
    enum transdlg_flags flags=tdf_enableback|tdf_enablekerns;
    if ( FVAnyCharSelected(fv)==-1 )
return;
    if ( FVAllSelected(fv))
	flags=tdf_enableback|tdf_enablekerns|tdf_defaultkerns;
    TransformDlgCreate(fv,FVTransFunc,getorigin,flags,cvt_none);
}