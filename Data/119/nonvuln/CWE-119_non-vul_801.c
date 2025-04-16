void SCPreparePopup(GWindow gw,SplineChar *sc,struct remap *remap, int localenc,
	int actualuni) {
/* This is for the popup which appears when you hover mouse over a character on main window */
    int upos=-1;
    char *msg, *msg_old;

    /* If a glyph is multiply mapped then the inbuild unicode enc may not be */
    /*  the actual one used to access the glyph */
    if ( remap!=NULL ) {
	while ( remap->infont!=-1 ) {
	    if ( localenc>=remap->infont && localenc<=remap->infont+(remap->lastenc-remap->firstenc) ) {
		localenc += remap->firstenc-remap->infont;
                break;
	    }
	    ++remap;
	}
    }

    if ( actualuni!=-1 )
	upos = actualuni;
    else if ( sc->unicodeenc!=-1 )
	upos = sc->unicodeenc;
#if HANYANG
    else if ( sc->compositionunit ) {
	if ( sc->jamo<19 )
	    upos = 0x1100+sc->jamo;
	else if ( sc->jamo<19+21 )
	    upos = 0x1161 + sc->jamo-19;
	else		/* Leave a hole for the blank char */
	    upos = 0x11a8 + sc->jamo-(19+21+1);
    }
#endif

    if ( upos == -1 ) {
	msg = smprintf( "%u 0x%x U+???? \"%.25s\" ",
		localenc, localenc,
		(sc->name == NULL) ? "" : sc->name );
    } else {
	/* unicode name or range name */
	char *uniname = unicode_name( upos );
	if( uniname == NULL ) uniname = strdup( UnicodeRange( upos ) );
	msg = smprintf ( "%u 0x%x U+%04X \"%.25s\" %.100s",
		localenc, localenc, upos,
		(sc->name == NULL) ? "" : sc->name, uniname );
	if ( uniname != NULL ) free( uniname ); uniname = NULL;

	/* annotation */
        char *uniannot = unicode_annot( upos );
        if( uniannot != NULL ) {
            msg_old = msg;
            msg = smprintf("%s\n%s", msg_old, uniannot);
            free(msg_old);
            free( uniannot );
        }
    }

    /* user comments */
    if ( sc->comment!=NULL ) {
        msg_old = msg;
        msg = smprintf("%s\n%s", msg_old, sc->comment);
        free(msg_old);
    }

    GGadgetPreparePopup8( gw, msg );
    free(msg);
}