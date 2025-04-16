static void FVResize(FontView *fv, GEvent *event) {
    extern int default_fv_row_count, default_fv_col_count;
    GRect pos,screensize;
    int topchar;

    if ( fv->colcnt!=0 )
	topchar = fv->rowoff*fv->colcnt;
    else if ( fv->b.sf->top_enc!=-1 && fv->b.sf->top_enc<fv->b.map->enccount )
	topchar = fv->b.sf->top_enc;
    else {
	/* Position on 'A' (or whatever they ask for) if it exists */
	topchar = SFFindSlot(fv->b.sf,fv->b.map,home_char,NULL);
	if ( topchar==-1 ) {
	    for ( topchar=0; topchar<fv->b.map->enccount; ++topchar )
		if ( fv->b.map->map[topchar]!=-1 && fv->b.sf->glyphs[fv->b.map->map[topchar]]!=NULL )
	    break;
	    if ( topchar==fv->b.map->enccount )
		topchar = 0;
	}
    }
    if ( !event->u.resize.sized )
	/* WM isn't responding to my resize requests, so no point in trying */;
    else if ( (event->u.resize.size.width-
		GDrawPointsToPixels(fv->gw,_GScrollBar_Width)-1)%fv->cbw!=0 ||
	    (event->u.resize.size.height-fv->mbh-fv->infoh-1)%fv->cbh!=0 ) {
	int cc = (event->u.resize.size.width+fv->cbw/2-
		GDrawPointsToPixels(fv->gw,_GScrollBar_Width)-1)/fv->cbw;
	int rc = (event->u.resize.size.height-fv->mbh-fv->infoh-1)/fv->cbh;
	if ( cc<=0 ) cc = 1;
	if ( rc<=0 ) rc = 1;
	GDrawGetSize(GDrawGetRoot(NULL),&screensize);
	if ( cc*fv->cbw+GDrawPointsToPixels(fv->gw,_GScrollBar_Width)>screensize.width )
	    --cc;
	if ( rc*fv->cbh+fv->mbh+fv->infoh+10>screensize.height )
	    --rc;
	GDrawResize(fv->gw,
		cc*fv->cbw+1+GDrawPointsToPixels(fv->gw,_GScrollBar_Width),
		rc*fv->cbh+1+fv->mbh+fv->infoh);
	/* somehow KDE loses this event of mine so to get even the vague effect */
	/*  we can't just return */
/*return;*/
    }

    pos.width = GDrawPointsToPixels(fv->gw,_GScrollBar_Width);
    pos.height = event->u.resize.size.height-fv->mbh-fv->infoh;
    pos.x = event->u.resize.size.width-pos.width; pos.y = fv->mbh+fv->infoh;
    GGadgetResize(fv->vsb,pos.width,pos.height);
    GGadgetMove(fv->vsb,pos.x,pos.y);
    pos.width = pos.x; pos.x = 0;
    GDrawResize(fv->v,pos.width,pos.height);

    fv->width = pos.width; fv->height = pos.height;
    fv->colcnt = (fv->width-1)/fv->cbw;
    if ( fv->colcnt<1 ) fv->colcnt = 1;
    fv->rowcnt = (fv->height-1)/fv->cbh;
    if ( fv->rowcnt<1 ) fv->rowcnt = 1;
    fv->rowltot = (fv->b.map->enccount+fv->colcnt-1)/fv->colcnt;

    GScrollBarSetBounds(fv->vsb,0,fv->rowltot,fv->rowcnt);
    fv->rowoff = topchar/fv->colcnt;
    if ( fv->rowoff>=fv->rowltot-fv->rowcnt )
        fv->rowoff = fv->rowltot-fv->rowcnt;
    if ( fv->rowoff<0 ) fv->rowoff =0;
    GScrollBarSetPos(fv->vsb,fv->rowoff);
    GDrawRequestExpose(fv->gw,NULL,true);
    GDrawRequestExpose(fv->v,NULL,true);

    if ( fv->rowcnt!=fv->b.sf->desired_row_cnt || fv->colcnt!=fv->b.sf->desired_col_cnt ) {
	default_fv_row_count = fv->rowcnt;
	default_fv_col_count = fv->colcnt;
	fv->b.sf->desired_row_cnt = fv->rowcnt;
	fv->b.sf->desired_col_cnt = fv->colcnt;
	SavePrefs(true);
    }
}