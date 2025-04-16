static void fllistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int anychars = FVAnyCharSelected(fv);
    FontView *fvs;
    int in_modal = (fv->b.container!=NULL && fv->b.container->funcs->is_modal);

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_GenerateTTC:
	    for ( fvs=fv_list; fvs!=NULL; fvs=(FontView *) (fvs->b.next) ) {
		if ( fvs!=fv )
	    break;
	    }
	    mi->ti.disabled = fvs==NULL;
	  break;
	  case MID_Revert:
	    mi->ti.disabled = fv->b.sf->origname==NULL || fv->b.sf->new;
	  break;
	  case MID_RevertToBackup:
	    /* We really do want to use filename here and origname above */
	    mi->ti.disabled = true;
	    if ( fv->b.sf->filename!=NULL ) {
		if ( fv->b.sf->backedup == bs_dontknow ) {
		    char *buf = malloc(strlen(fv->b.sf->filename)+20);
		    strcpy(buf,fv->b.sf->filename);
		    if ( fv->b.sf->compression!=0 )
			strcat(buf,compressors[fv->b.sf->compression-1].ext);
		    strcat(buf,"~");
		    if ( access(buf,F_OK)==0 )
			fv->b.sf->backedup = bs_backedup;
		    else
			fv->b.sf->backedup = bs_not;
		    free(buf);
		}
		if ( fv->b.sf->backedup == bs_backedup )
		    mi->ti.disabled = false;
	    }
	  break;
	  case MID_RevertGlyph:
	    mi->ti.disabled = fv->b.sf->origname==NULL || fv->b.sf->sfd_version<2 || anychars==-1 || fv->b.sf->compression!=0;
	  break;
	  case MID_Recent:
	    mi->ti.disabled = !RecentFilesAny();
	  break;
	  case MID_ScriptMenu:
	    mi->ti.disabled = script_menu_names[0]==NULL;
	  break;
	  case MID_Print:
	    mi->ti.disabled = fv->b.sf->onlybitmaps || in_modal;
	  break;
	}
    }
}