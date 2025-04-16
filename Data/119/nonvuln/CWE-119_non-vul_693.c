static void FVReselect(FontView *fv, int newpos) {
    int i;

    if ( newpos<0 ) newpos = 0;
    else if ( newpos>=fv->b.map->enccount ) newpos = fv->b.map->enccount-1;

    if ( fv->pressed_pos<fv->end_pos ) {
	if ( newpos>fv->end_pos ) {
	    for ( i=fv->end_pos+1; i<=newpos; ++i ) if ( !fv->b.selected[i] ) {
		fv->b.selected[i] = fv->sel_index;
		FVToggleCharSelected(fv,i);
	    }
	} else if ( newpos<fv->pressed_pos ) {
	    for ( i=fv->end_pos; i>fv->pressed_pos; --i ) if ( fv->b.selected[i] ) {
		fv->b.selected[i] = false;
		FVToggleCharSelected(fv,i);
	    }
	    for ( i=fv->pressed_pos-1; i>=newpos; --i ) if ( !fv->b.selected[i] ) {
		fv->b.selected[i] = fv->sel_index;
		FVToggleCharSelected(fv,i);
	    }
	} else {
	    for ( i=fv->end_pos; i>newpos; --i ) if ( fv->b.selected[i] ) {
		fv->b.selected[i] = false;
		FVToggleCharSelected(fv,i);
	    }
	}
    } else {
	if ( newpos<fv->end_pos ) {
	    for ( i=fv->end_pos-1; i>=newpos; --i ) if ( !fv->b.selected[i] ) {
		fv->b.selected[i] = fv->sel_index;
		FVToggleCharSelected(fv,i);
	    }
	} else if ( newpos>fv->pressed_pos ) {
	    for ( i=fv->end_pos; i<fv->pressed_pos; ++i ) if ( fv->b.selected[i] ) {
		fv->b.selected[i] = false;
		FVToggleCharSelected(fv,i);
	    }
	    for ( i=fv->pressed_pos+1; i<=newpos; ++i ) if ( !fv->b.selected[i] ) {
		fv->b.selected[i] = fv->sel_index;
		FVToggleCharSelected(fv,i);
	    }
	} else {
	    for ( i=fv->end_pos; i<newpos; ++i ) if ( fv->b.selected[i] ) {
		fv->b.selected[i] = false;
		FVToggleCharSelected(fv,i);
	    }
	}
    }
    fv->end_pos = newpos;
    if ( newpos>=0 && newpos<fv->b.map->enccount && (i = fv->b.map->map[newpos])!=-1 &&
	    fv->b.sf->glyphs[i]!=NULL &&
	    fv->b.sf->glyphs[i]->unicodeenc>=0 && fv->b.sf->glyphs[i]->unicodeenc<0x10000 )
	GInsCharSetChar(fv->b.sf->glyphs[i]->unicodeenc);
}