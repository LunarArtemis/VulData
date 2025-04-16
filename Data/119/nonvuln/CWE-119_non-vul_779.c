static void enlistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int i, gid;
    SplineFont *sf = fv->b.sf;
    EncMap *map = fv->b.map;
    int anyglyphs = false;

    for ( i=map->enccount-1; i>=0 ; --i )
	if ( fv->b.selected[i] && (gid=map->map[i])!=-1 )
	    anyglyphs = true;

    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_Compact:
	    mi->ti.checked = fv->b.normal!=NULL;
	  break;
	case MID_HideNoGlyphSlots:
	    break;
	  case MID_Reencode: case MID_ForceReencode:
	    mi->ti.disabled = fv->b.cidmaster!=NULL;
	  break;
	  case MID_DetachGlyphs: case MID_DetachAndRemoveGlyphs:
	    mi->ti.disabled = !anyglyphs;
	  break;
	  case MID_RemoveUnused:
	    gid = map->enccount>0 ? map->map[map->enccount-1] : -1;
	    mi->ti.disabled = gid!=-1 && SCWorthOutputting(sf->glyphs[gid]);
	  break;
	  case MID_MakeFromFont:
	    mi->ti.disabled = fv->b.cidmaster!=NULL || map->enccount>1024 || map->enc!=&custom;
	  break;
	  case MID_RemoveEncoding:
	  break;
	  case MID_DisplayByGroups:
	    mi->ti.disabled = fv->b.cidmaster!=NULL || group_root==NULL;
	  break;
	  case MID_NameGlyphs:
	    mi->ti.disabled = fv->b.normal!=NULL || fv->b.cidmaster!=NULL;
	  break;
	  case MID_RenameGlyphs: case MID_SaveNamelist:
	    mi->ti.disabled = fv->b.cidmaster!=NULL;
	  break;
	}
    }
}