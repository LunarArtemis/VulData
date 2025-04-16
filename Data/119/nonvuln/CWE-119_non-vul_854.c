static void FVMenuClose(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( fv->b.container )
	(fv->b.container->funcs->doClose)(fv->b.container);
    else
	_FVMenuClose(fv);
}