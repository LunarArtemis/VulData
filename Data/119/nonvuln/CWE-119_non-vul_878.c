static void gs_charEvent(struct fvcontainer *fvc,void *event) {
    struct gsd *gs = (struct gsd *) fvc;
    FVChar(gs->fv,event);
}