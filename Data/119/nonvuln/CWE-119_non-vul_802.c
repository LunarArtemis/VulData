static void FVSimplify(FontView *fv,int type) {
    static struct simplifyinfo smpls[] = {
	    { sf_normal, 0, 0, 0, 0, 0, 0 },
	    { sf_normal,.75,.05,0,-1, 0, 0 },
	    { sf_normal,.75,.05,0,-1, 0, 0 }};
    struct simplifyinfo *smpl = &smpls[type+1];

    if ( smpl->linelenmax==-1 || (type==0 && !smpl->set_as_default)) {
	smpl->err = (fv->b.sf->ascent+fv->b.sf->descent)/1000.;
	smpl->linelenmax = (fv->b.sf->ascent+fv->b.sf->descent)/100.;
    }

    if ( type==1 ) {
	if ( !SimplifyDlg(fv->b.sf,smpl))
return;
	if ( smpl->set_as_default )
	    smpls[1] = *smpl;
    }
    _FVSimplify((FontViewBase *) fv,smpl);
}