static void FontViewFinish() {
    if (!fontview_ready) return;
    mb2FreeGetText(mblist);
    mbFreeGetText(fvpopupmenu);
}