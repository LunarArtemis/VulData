static void FVCopyInnards(FontView *fv,GRect *pos,int infoh,
	FontView *fvorig,GWindow dw, int def_layer, struct fvcontainer *kf) {

    fv->notactive = true;
    fv->gw = dw;
    fv->infoh = infoh;
    fv->b.container = kf;
    fv->rowcnt = 4; fv->colcnt = 16;
    fv->b.active_layer = def_layer;
    FVCreateInnards(fv,pos);
    memcpy(fv->b.selected,fvorig->b.selected,fv->b.map->enccount);
    fv->rowoff = (fvorig->rowoff*fvorig->colcnt)/fv->colcnt;
}