static void FontViewInit(void) {
    // static int done = false; // superseded by fontview_ready.

    if ( fontview_ready )
return;

    fontview_ready = true;

    mb2DoGetText(mblist);
    mbDoGetText(fvpopupmenu);
    atexit(&FontViewFinishNonStatic);
}