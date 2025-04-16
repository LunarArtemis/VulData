int _FVMenuSave(FontView *fv) {
    int ret = 0;
    SplineFont *sf = fv->b.cidmaster?fv->b.cidmaster:
		    fv->b.sf->mm!=NULL?fv->b.sf->mm->normal:
			    fv->b.sf;

    if ( sf->filename==NULL || IsBackupName(sf->filename))
	ret = _FVMenuSaveAs(fv);
    else {
	FVFlattenAllBitmapSelections(fv);
	if ( !SFDWriteBak(sf->filename,sf,fv->b.map,fv->b.normal) )
	    ff_post_error(_("Save Failed"),_("Save Failed"));
	else {
	    SplineFontSetUnChanged(sf);
	    ret = true;
	}
    }
return( ret );
}