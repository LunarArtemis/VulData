static void FVMenuAutotrace(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *e) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    GCursor ct=0;

    if ( fv->v!=NULL ) {
	ct = GDrawGetCursor(fv->v);
	GDrawSetCursor(fv->v,ct_watch);
	GDrawSync(NULL);
	GDrawProcessPendingEvents(NULL);
    }
    FVAutoTrace(&fv->b,e!=NULL && (e->u.mouse.state&ksm_shift));
    if ( fv->v!=NULL )
	GDrawSetCursor(fv->v,ct);
}