SplineChar *FVMakeChar(FontView *fv,int enc) {
    SplineFont *sf = fv->b.sf;
    SplineChar *base_sc = SFMakeChar(sf,fv->b.map,enc), *feat_sc = NULL;
    int feat_gid = FeatureTrans(fv,enc);

    if ( fv->cur_subtable==NULL )
return( base_sc );

    if ( feat_gid==-1 ) {
	int uni = -1;
	FeatureScriptLangList *fl = fv->cur_subtable->lookup->features;

	if ( base_sc->unicodeenc>=0x600 && base_sc->unicodeenc<=0x6ff &&
		fl!=NULL &&
		(fl->featuretag == CHR('i','n','i','t') ||
		 fl->featuretag == CHR('m','e','d','i') ||
		 fl->featuretag == CHR('f','i','n','a') ||
		 fl->featuretag == CHR('i','s','o','l')) ) {
	    uni = fl->featuretag == CHR('i','n','i','t') ? ArabicForms[base_sc->unicodeenc-0x600].initial  :
		  fl->featuretag == CHR('m','e','d','i') ? ArabicForms[base_sc->unicodeenc-0x600].medial   :
		  fl->featuretag == CHR('f','i','n','a') ? ArabicForms[base_sc->unicodeenc-0x600].final    :
		  fl->featuretag == CHR('i','s','o','l') ? ArabicForms[base_sc->unicodeenc-0x600].isolated :
		  -1;
	    feat_sc = SFGetChar(sf,uni,NULL);
	    if ( feat_sc!=NULL )
return( feat_sc );
	}
	feat_sc = SFSplineCharCreate(sf);
	feat_sc->unicodeenc = uni;
	if ( uni!=-1 ) {
	    feat_sc->name = malloc(8);
	    feat_sc->unicodeenc = uni;
	    sprintf( feat_sc->name,"uni%04X", uni );
	} else if ( fv->cur_subtable->suffix!=NULL ) {
	    feat_sc->name = malloc(strlen(base_sc->name)+strlen(fv->cur_subtable->suffix)+2);
	    sprintf( feat_sc->name, "%s.%s", base_sc->name, fv->cur_subtable->suffix );
	} else if ( fl==NULL ) {
	    feat_sc->name = strconcat(base_sc->name,".unknown");
	} else if ( fl->ismac ) {
	    /* mac feature/setting */
	    feat_sc->name = malloc(strlen(base_sc->name)+14);
	    sprintf( feat_sc->name,"%s.m%d_%d", base_sc->name,
		    (int) (fl->featuretag>>16),
		    (int) ((fl->featuretag)&0xffff) );
	} else {
	    /* OpenType feature tag */
	    feat_sc->name = malloc(strlen(base_sc->name)+6);
	    sprintf( feat_sc->name,"%s.%c%c%c%c", base_sc->name,
		    (int) (fl->featuretag>>24),
		    (int) ((fl->featuretag>>16)&0xff),
		    (int) ((fl->featuretag>>8)&0xff),
		    (int) ((fl->featuretag)&0xff) );
	}
	SFAddGlyphAndEncode(sf,feat_sc,fv->b.map,fv->b.map->enccount);
	AddSubPST(base_sc,fv->cur_subtable,feat_sc->name);
return( feat_sc );
    } else
return( base_sc );
}