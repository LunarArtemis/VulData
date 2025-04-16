static void _FVMenuChangeChar(FontView *fv,int mid ) {
    SplineFont *sf = fv->b.sf;
    EncMap *map = fv->b.map;
    int pos = FVAnyCharSelected(fv);

    if ( pos>=0 ) {
	if ( mid==MID_Next )
	    ++pos;
	else if ( mid==MID_Prev )
	    --pos;
	else if ( mid==MID_NextDef ) {
	    for ( ++pos; pos<map->enccount &&
		    (map->map[pos]==-1 || !SCWorthOutputting(sf->glyphs[map->map[pos]]) ||
			(fv->show!=fv->filled && fv->show->glyphs[map->map[pos]]==NULL ));
		    ++pos );
	    if ( pos>=map->enccount ) {
		int selpos = FVAnyCharSelected(fv);
		char *iconv_name = map->enc->iconv_name ? map->enc->iconv_name :
			map->enc->enc_name;
		if ( strstr(iconv_name,"2022")!=NULL && selpos<0x2121 )
		    pos = 0x2121;
		else if ( strstr(iconv_name,"EUC")!=NULL && selpos<0xa1a1 )
		    pos = 0xa1a1;
		else if ( map->enc->is_tradchinese ) {
		    if ( strstrmatch(map->enc->enc_name,"HK")!=NULL &&
			    selpos<0x8140 )
			pos = 0x8140;
		    else
			pos = 0xa140;
		} else if ( map->enc->is_japanese ) {
		    if ( strstrmatch(iconv_name,"SJIS")!=NULL ||
			    (strstrmatch(iconv_name,"JIS")!=NULL && strstrmatch(iconv_name,"SHIFT")!=NULL )) {
			if ( selpos<0x8100 )
			    pos = 0x8100;
			else if ( selpos<0xb000 )
			    pos = 0xb000;
		    }
		} else if ( map->enc->is_korean ) {
		    if ( strstrmatch(iconv_name,"JOHAB")!=NULL ) {
			if ( selpos<0x8431 )
			    pos = 0x8431;
		    } else {	/* Wansung, EUC-KR */
			if ( selpos<0xa1a1 )
			    pos = 0xa1a1;
		    }
		} else if ( map->enc->is_simplechinese ) {
		    if ( strmatch(iconv_name,"EUC-CN")==0 && selpos<0xa1a1 )
			pos = 0xa1a1;
		}
		if ( pos>=map->enccount )
return;
	    }
	} else if ( mid==MID_PrevDef ) {
	    for ( --pos; pos>=0 &&
		    (map->map[pos]==-1 || !SCWorthOutputting(sf->glyphs[map->map[pos]]) ||
			(fv->show!=fv->filled && fv->show->glyphs[map->map[pos]]==NULL ));
		    --pos );
	    if ( pos<0 )
return;
	}
    }
    if ( pos<0 ) pos = map->enccount-1;
    else if ( pos>= map->enccount ) pos = 0;
    if ( pos>=0 && pos<map->enccount )
	FVChangeChar(fv,pos);
}