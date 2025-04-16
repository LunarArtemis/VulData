void FVDrawInfo(FontView *fv,GWindow pixmap, GEvent *event) {
    GRect old, r;
    Color bg = GDrawGetDefaultBackground(GDrawGetDisplayOfWindow(pixmap));
    Color fg = fvglyphinfocol;
    SplineChar *sc, dummy;
    SplineFont *sf = fv->b.sf;
    EncMap *map = fv->b.map;
    int gid, uni, localenc;
    GString *output = g_string_new( "" );
    gchar *uniname = NULL;

    if ( event->u.expose.rect.y+event->u.expose.rect.height<=fv->mbh ) {
        g_string_free( output, TRUE ); output = NULL;
	return;
    }

    GDrawSetFont(pixmap,fv->fontset[0]);
    GDrawPushClip(pixmap,&event->u.expose.rect,&old);

    r.x = 0; r.width = fv->width; r.y = fv->mbh; r.height = fv->infoh;
    GDrawFillRect(pixmap,&r,bg);
    if ( fv->end_pos>=map->enccount || fv->pressed_pos>=map->enccount ||
	    fv->end_pos<0 || fv->pressed_pos<0 )
	fv->end_pos = fv->pressed_pos = -1;	/* Can happen after reencoding */
    if ( fv->end_pos == -1 ) {
        g_string_free( output, TRUE ); output = NULL;
	GDrawPopClip(pixmap,&old);
	return;
    }

    localenc = fv->end_pos;
    if ( map->remap!=NULL ) {
	struct remap *remap = map->remap;
	while ( remap->infont!=-1 ) {
	    if ( localenc>=remap->infont && localenc<=remap->infont+(remap->lastenc-remap->firstenc) ) {
		localenc += remap->firstenc-remap->infont;
		break;
	    }
	    ++remap;
	}
    }
    g_string_printf( output, "%d (0x%x) ", localenc, localenc );

    sc = (gid=fv->b.map->map[fv->end_pos])!=-1 ? sf->glyphs[gid] : NULL;
    if ( fv->b.cidmaster==NULL || fv->b.normal==NULL || sc==NULL )
	SCBuildDummy(&dummy,sf,fv->b.map,fv->end_pos);
    else
	dummy = *sc;
    if ( sc==NULL ) sc = &dummy;
    uni = dummy.unicodeenc!=-1 ? dummy.unicodeenc : sc->unicodeenc;

    /* last resort at guessing unicode code point from partial name */
    if ( uni == -1 ) {
	char *pt = strchr( sc->name, '.' );
	if( pt != NULL ) {
	    gchar *buf = g_strndup( (const gchar *) sc->name, pt - sc->name );
	    uni = UniFromName( (char *) buf, fv->b.sf->uni_interp, map->enc );
	    g_free( buf );
	}
    }

    if ( uni != -1 )
	g_string_append_printf( output, "U+%04X", uni );
    else {
	output = g_string_append( output, "U+????" );
    }

    /* postscript name */
    g_string_append_printf( output, " \"%s\" ", sc->name );

    /* code point name or range name */
    if( uni != -1 ) {
	uniname = (gchar *) unicode_name( uni );
	if ( uniname == NULL ) {
	    uniname = g_strdup( UnicodeRange( uni ) );
	}
    }

    if ( uniname != NULL ) {
	output = g_string_append( output, uniname );
	g_free( uniname );
    }

    GDrawDrawText8( pixmap, 10, fv->mbh+fv->lab_as, output->str, -1, fg );
    g_string_free( output, TRUE ); output = NULL;
    GDrawPopClip( pixmap, &old );
    return;
}