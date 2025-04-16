static void FVChangeDisplayFont(FontView *fv,BDFFont *bdf) {
    int samesize=0;
    int rcnt, ccnt;
    int oldr, oldc;
    int first_time = fv->show==NULL;

    if ( fv->v==NULL )			/* Can happen in scripts */
return;

    if ( fv->show!=bdf ) {
	oldc = fv->cbw*fv->colcnt;
	oldr = fv->cbh*fv->rowcnt;

	fv->show = bdf;
	fv->b.active_bitmap = bdf==fv->filled ? NULL : bdf;
	if ( fv->user_requested_magnify!=-1 )
	    fv->magnify=fv->user_requested_magnify;
	else if ( bdf->pixelsize<20 ) {
	    if ( bdf->pixelsize<=9 )
		fv->magnify = 3;
	    else
		fv->magnify = 2;
	    samesize = ( fv->show && fv->cbw == (bdf->pixelsize*fv->magnify)+1 );
	} else
	    fv->magnify = 1;
	if ( !first_time && fv->cbw == fv->magnify*bdf->pixelsize+1 )
	    samesize = true;
	fv->cbw = (bdf->pixelsize*fv->magnify)+1;
	fv->cbh = (bdf->pixelsize*fv->magnify)+1+fv->lab_height+1;
	fv->resize_expected = !samesize;
	ccnt = fv->b.sf->desired_col_cnt;
	rcnt = fv->b.sf->desired_row_cnt;
	if ((( bdf->pixelsize<=fv->b.sf->display_size || bdf->pixelsize<=-fv->b.sf->display_size ) &&
		 fv->b.sf->top_enc!=-1 /* Not defaulting */ ) ||
		bdf->pixelsize<=48 ) {
	    /* use the desired sizes */
	} else {
	    if ( bdf->pixelsize>48 ) {
		ccnt = 8;
		rcnt = 2;
	    } else if ( bdf->pixelsize>=96 ) {
		ccnt = 4;
		rcnt = 1;
	    }
	    if ( !first_time ) {
		if ( ccnt < oldc/fv->cbw )
		    ccnt = oldc/fv->cbw;
		if ( rcnt < oldr/fv->cbh )
		    rcnt = oldr/fv->cbh;
	    }
	}
	if ( samesize ) {
	    GDrawRequestExpose(fv->v,NULL,false);
	} else if ( fv->b.container!=NULL && fv->b.container->funcs->doResize!=NULL ) {
	    (fv->b.container->funcs->doResize)(fv->b.container,&fv->b,
		    ccnt*fv->cbw+1+GDrawPointsToPixels(fv->gw,_GScrollBar_Width),
		    rcnt*fv->cbh+1+fv->mbh+fv->infoh);
	} else {
	    GDrawResize(fv->gw,
		    ccnt*fv->cbw+1+GDrawPointsToPixels(fv->gw,_GScrollBar_Width),
		    rcnt*fv->cbh+1+fv->mbh+fv->infoh);
	}
    }
}