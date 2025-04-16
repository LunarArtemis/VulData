static void FVShowInfo(FontView *fv) {
    GRect r;

    if ( fv->v==NULL )			/* Can happen in scripts */
return;

    r.x = 0; r.width = fv->width; r.y = fv->mbh; r.height = fv->infoh;
    GDrawRequestExpose(fv->gw,&r,false);
}