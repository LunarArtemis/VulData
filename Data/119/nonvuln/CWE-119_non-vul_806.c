static void FVWindowMenuBuild(GWindow gw, struct gmenuitem *mi, GEvent *e) {
    FontView *fv = (FontView *) GDrawGetUserData(gw);
    int anychars = FVAnyCharSelected(fv);
    struct gmenuitem *wmi;
    int in_modal = (fv->b.container!=NULL && fv->b.container->funcs->is_modal);

    WindowMenuBuild(gw,mi,e);
    for ( wmi = mi->sub; wmi->ti.text!=NULL || wmi->ti.line ; ++wmi ) {
	switch ( wmi->mid ) {
	  case MID_OpenOutline:
	    wmi->ti.disabled = anychars==-1 || in_modal;
	  break;
	  case MID_OpenBitmap:
	    wmi->ti.disabled = anychars==-1 || fv->b.sf->bitmaps==NULL || in_modal;
	  break;
	  case MID_OpenMetrics:
	    wmi->ti.disabled = in_modal;
	  break;
	  case MID_Warnings:
	    wmi->ti.disabled = ErrorWindowExists();
	  break;
	}
    }
}