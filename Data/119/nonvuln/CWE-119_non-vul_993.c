static void GCompletionDestroy(GCompletionField *gc) {
    int i;

    if ( gc->choice_popup!=NULL ) {
	GWindow cp = gc->choice_popup;
	gc->choice_popup = NULL;
	GDrawSetUserData(cp,NULL);
	GDrawDestroyWindow(cp);
    }
    if ( gc->choices!=NULL ) {
	for ( i=0; gc->choices[i]!=NULL; ++i )
	    free(gc->choices[i]);
	free(gc->choices);
	gc->choices = NULL;
    }
}