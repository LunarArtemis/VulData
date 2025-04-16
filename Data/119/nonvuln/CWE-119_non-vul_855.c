static void _MenuExit(void *UNUSED(junk)) {

    FontView *fv, *next;

#ifndef _NO_PYTHON
    python_call_onClosingFunctions();
#endif

    LastFonts_Save();
    for ( fv = fv_list; fv!=NULL; fv = next )
    {
	next = (FontView *) (fv->b.next);
	if ( !_FVMenuClose(fv))
	    return;
	if ( fv->b.nextsame!=NULL || fv->b.sf->fv!=&fv->b )
	{
	    GDrawSync(NULL);
	    GDrawProcessPendingEvents(NULL);
	}
    }
    GDrawSync(NULL);
    GDrawProcessPendingEvents(NULL);
    exit(0);
}