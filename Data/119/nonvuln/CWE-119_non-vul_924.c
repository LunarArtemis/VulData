static void FVMenuBitmaps(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    BitmapDlg(fv,NULL,mi->mid==MID_RemoveBitmaps?-1:(mi->mid==MID_AvailBitmaps) );
}