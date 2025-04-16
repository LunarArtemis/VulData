static void FVScrollToGID(FontView *fv,int gid) {
    FVScrollToChar(fv,fv->b.map->backmap[gid]);
}