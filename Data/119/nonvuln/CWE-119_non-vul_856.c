static void edlistcheck(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int pos = FVAnyCharSelected(fv), i, gid;
    int not_pasteable = pos==-1 ||
		    (!CopyContainsSomething() &&
#ifndef _NO_LIBPNG
		    !GDrawSelectionHasType(fv->gw,sn_clipboard,"image/png") &&
#endif
		    !GDrawSelectionHasType(fv->gw,sn_clipboard,"image/svg+xml") &&
		    !GDrawSelectionHasType(fv->gw,sn_clipboard,"image/svg-xml") &&
		    !GDrawSelectionHasType(fv->gw,sn_clipboard,"image/svg") &&
		    !GDrawSelectionHasType(fv->gw,sn_clipboard,"image/bmp") &&
		    !GDrawSelectionHasType(fv->gw,sn_clipboard,"image/eps") &&
		    !GDrawSelectionHasType(fv->gw,sn_clipboard,"image/ps"));
    RefChar *base = CopyContainsRef(fv->b.sf);
    int base_enc = base!=NULL ? fv->b.map->backmap[base->orig_pos] : -1;


    for ( mi = mi->sub; mi->ti.text!=NULL || mi->ti.line ; ++mi ) {
	switch ( mi->mid ) {
	  case MID_Paste: case MID_PasteInto:
	    mi->ti.disabled = not_pasteable;
	  break;
	  case MID_PasteAfter:
	    mi->ti.disabled = not_pasteable || pos<0;
	  break;
	  case MID_SameGlyphAs:
	    mi->ti.disabled = not_pasteable || base==NULL || fv->b.cidmaster!=NULL ||
		    base_enc==-1 ||
		    fv->b.selected[base_enc];	/* Can't be self-referential */
	  break;
	  case MID_Join:
	  case MID_Cut: case MID_Copy: case MID_Clear:
	  case MID_CopyWidth: case MID_CopyLBearing: case MID_CopyRBearing:
	  case MID_CopyRef: case MID_UnlinkRef:
	  case MID_RemoveUndoes: case MID_CopyFgToBg: case MID_CopyL2L:
	    mi->ti.disabled = pos==-1;
	  break;
	  case MID_RplRef:
	  case MID_CorrectRefs:
	    mi->ti.disabled = pos==-1 || fv->b.cidmaster!=NULL || fv->b.sf->multilayer;
	  break;
	  case MID_CopyLookupData:
	    mi->ti.disabled = pos==-1 || (fv->b.sf->gpos_lookups==NULL && fv->b.sf->gsub_lookups==NULL);
	  break;
	  case MID_CopyVWidth:
	    mi->ti.disabled = pos==-1 || !fv->b.sf->hasvmetrics;
	  break;
	  case MID_ClearBackground:
	    mi->ti.disabled = true;
	    if ( pos!=-1 && !( onlycopydisplayed && fv->filled!=fv->show )) {
		for ( i=0; i<fv->b.map->enccount; ++i )
		    if ( fv->b.selected[i] && (gid = fv->b.map->map[i])!=-1 &&
			    fv->b.sf->glyphs[gid]!=NULL )
			if ( fv->b.sf->glyphs[gid]->layers[ly_back].images!=NULL ||
				fv->b.sf->glyphs[gid]->layers[ly_back].splines!=NULL ) {
			    mi->ti.disabled = false;
		break;
			}
	    }
	  break;
	  case MID_Undo:
	    for ( i=0; i<fv->b.map->enccount; ++i )
		if ( fv->b.selected[i] && (gid = fv->b.map->map[i])!=-1 &&
			fv->b.sf->glyphs[gid]!=NULL )
		    if ( fv->b.sf->glyphs[gid]->layers[fv->b.active_layer].undoes!=NULL )
	    break;
	    mi->ti.disabled = i==fv->b.map->enccount;
	  break;
	  case MID_Redo:
	    for ( i=0; i<fv->b.map->enccount; ++i )
		if ( fv->b.selected[i] && (gid = fv->b.map->map[i])!=-1 &&
			fv->b.sf->glyphs[gid]!=NULL )
		    if ( fv->b.sf->glyphs[gid]->layers[fv->b.active_layer].redoes!=NULL )
	    break;
	    mi->ti.disabled = i==fv->b.map->enccount;
	  break;
	case MID_UndoFontLevel:
	    mi->ti.disabled = dlist_isempty( (struct dlistnode **)&fv->b.sf->undoes );
	    break;
	}
    }
}