static void FV_ChangeDisplayBitmap(FontView *fv,BDFFont *bdf) {
    FVChangeDisplayFont(fv,bdf);
    if (fv->show != NULL) {
        fv->b.sf->display_size = fv->show->pixelsize;
    } else {
        fv->b.sf->display_size = 1;
    }
}