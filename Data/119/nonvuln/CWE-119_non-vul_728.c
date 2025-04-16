static void FV_ChangeGID(FontView *fv,int gid) {
    FVChangeChar(fv,fv->b.map->backmap[gid]);
}