static void FV_ReattachCVs(SplineFont *old,SplineFont *new) {
    int i, j, pos;
    CharView *cv, *cvnext;
    SplineFont *sub;

    for ( i=0; i<old->glyphcnt; ++i ) {
	if ( old->glyphs[i]!=NULL && old->glyphs[i]->views!=NULL ) {
	    if ( new->subfontcnt==0 ) {
		pos = SFFindExistingSlot(new,old->glyphs[i]->unicodeenc,old->glyphs[i]->name);
		sub = new;
	    } else {
		pos = -1;
		for ( j=0; j<new->subfontcnt && pos==-1 ; ++j ) {
		    sub = new->subfonts[j];
		    pos = SFFindExistingSlot(sub,old->glyphs[i]->unicodeenc,old->glyphs[i]->name);
		}
	    }
	    if ( pos==-1 ) {
		for ( cv=(CharView *) (old->glyphs[i]->views); cv!=NULL; cv = cvnext ) {
		    cvnext = (CharView *) (cv->b.next);
		    GDrawDestroyWindow(cv->gw);
		}
	    } else {
		for ( cv=(CharView *) (old->glyphs[i]->views); cv!=NULL; cv = cvnext ) {
		    cvnext = (CharView *) (cv->b.next);
		    CVChangeSC(cv,sub->glyphs[pos]);
		    cv->b.layerheads[dm_grid] = &new->grid;
		}
	    }
	    GDrawProcessPendingEvents(NULL);		/* Don't want to many destroy_notify events clogging up the queue */
	}
    }
}