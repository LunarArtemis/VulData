static void mmlistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i, base, j;
    MMSet *mm = fv->b.sf->mm;
    SplineFont *sub;
    GMenuItem2 *mml;

    for ( i=0; mmlist[i].mid!=MID_ChangeMMBlend; ++i );
    base = i+2;
    if ( mm==NULL )
	mml = mmlist;
    else {
	mml = calloc(base+mm->instance_count+2,sizeof(GMenuItem2));
	memcpy(mml,mmlist,sizeof(mmlist));
	mml[base-1].ti.fg = mml[base-1].ti.bg = COLOR_DEFAULT;
	mml[base-1].ti.line = true;
	for ( j = 0, i=base; j<mm->instance_count+1; ++i, ++j ) {
	    if ( j==0 )
		sub = mm->normal;
	    else
		sub = mm->instances[j-1];
	    mml[i].ti.text = uc_copy(sub->fontname);
	    mml[i].ti.checkable = true;
	    mml[i].ti.checked = sub==fv->b.sf;
	    mml[i].ti.userdata = sub;
	    mml[i].invoke = FVMenuShowSubFont;
	    mml[i].ti.fg = mml[i].ti.bg = COLOR_DEFAULT;
	}
    }
    GMenuItemArrayFree(mi->sub);
    mi->sub = GMenuItem2ArrayCopy(mml,NULL);
    if ( mml!=mmlist ) {
	for ( i=base; mml[i].ti.text!=NULL; ++i )
	    free( mml[i].ti.text);
	free(mml);
    }

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_CreateMM:
	    mi->ti.disabled = false;
	  break;
	  case MID_MMInfo: case MID_MMValid: case MID_BlendToNew:
	    mi->ti.disabled = mm==NULL;
	  break;
	  case MID_ChangeMMBlend:
	    mi->ti.disabled = mm==NULL || mm->apple;
	  break;
	}
    }
}