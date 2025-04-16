static void gs_doClose(struct fvcontainer *fvc) {
    struct gsd *gs = (struct gsd *) fvc;
    gs->done = true;
}