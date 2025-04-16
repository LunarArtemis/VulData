void MenuSaveAll(GWindow UNUSED(base), struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv;

    for ( fv = fv_list; fv!=NULL; fv = (FontView *) (fv->b.next) ) {
	if ( SFAnyChanged(fv->b.sf) && !_FVMenuSave(fv))
return;
    }
}