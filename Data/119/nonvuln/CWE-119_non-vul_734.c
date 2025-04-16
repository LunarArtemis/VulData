static void FontView_ReformatOne(FontView *fv) {
    FontView *fvs;

    if ( fv->v==NULL || fv->colcnt==0 )	/* Can happen in scripts */
return;

    GDrawSetCursor(fv->v,ct_watch);
    fv->rowltot = (fv->b.map->enccount+fv->colcnt-1)/fv->colcnt;
    GScrollBarSetBounds(fv->vsb,0,fv->rowltot,fv->rowcnt);
    if ( fv->rowoff>fv->rowltot-fv->rowcnt ) {
        fv->rowoff = fv->rowltot-fv->rowcnt;
	if ( fv->rowoff<0 ) fv->rowoff =0;
	GScrollBarSetPos(fv->vsb,fv->rowoff);
    }
    for ( fvs=(FontView *) (fv->b.sf->fv); fvs!=NULL; fvs=(FontView *) (fvs->b.nextsame) )
	if ( fvs!=fv && fvs->b.sf==fv->b.sf )
    break;
    GDrawRequestExpose(fv->v,NULL,false);
    GDrawSetCursor(fv->v,ct_pointer);
}