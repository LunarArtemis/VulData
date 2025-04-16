static void FVMenuOverlap(GWindow gw, struct gmenuitem *mi, GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( fv->b.sf->onlybitmaps )
return;

    /* We know it's more likely that we'll find a problem in the overlap code */
    /*  than anywhere else, so let's save the current state against a crash */
    DoAutoSaves();

    FVOverlap(&fv->b,mi->mid==MID_RmOverlap ? over_remove :
		 mi->mid==MID_Intersection ? over_intersect :
		      over_findinter);
}