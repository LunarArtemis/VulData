static void FVMenuShowBitmap(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    BDFFont *bdf = mi->ti.userdata;

    FV_ChangeDisplayBitmap(fv,bdf);		/* Let's not change any of the others */
}