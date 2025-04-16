static void FVMenuUndoFontLevel(GWindow gw,struct gmenuitem *mi,GEvent *e) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    FontViewBase * fvb = (FontViewBase *) fv;
    SplineFont *sf = fvb->sf;

    if( !sf->undoes )
	return;

    struct sfundoes *undo = sf->undoes;
//    printf("font level undo msg:%s\n", undo->msg );
    SFUndoPerform( undo, sf );
    SFUndoRemoveAndFree( sf, undo );
}