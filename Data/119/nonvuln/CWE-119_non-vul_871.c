static void gs_doResize(struct fvcontainer *fvc, FontViewBase *UNUSED(fvb),
	int width, int height) {
    struct gsd *gs = (struct gsd *) fvc;
    /*FontView *fv = (FontView *) fvb;*/
    GRect size;

    memset(&size,0,sizeof(size));
    size.width = width; size.height = height;
    GGadgetSetDesiredSize(GWidgetGetControl(gs->gw,CID_Guts),
	    NULL,&size);
    GHVBoxFitWindow(GWidgetGetControl(gs->gw,CID_TopBox));
}