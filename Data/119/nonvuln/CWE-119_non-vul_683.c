static void FVMenuPrint(GWindow gw, struct gmenuitem *UNUSED(mi), GEvent *UNUSED(e)) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);

    if ( fv->b.container!=NULL && fv->b.container->funcs->is_modal )
return;
    PrintFFDlg(fv,NULL,NULL);
}