static int md_e_h(GWindow gw, GEvent *e) {
    if ( e->type==et_close ) {
	struct md_data *d = GDrawGetUserData(gw);
	d->done = true;
    } else if ( e->type==et_controlevent && e->u.control.subtype == et_buttonactivate ) {
	struct md_data *d = GDrawGetUserData(gw);
	static int masks[] = { fvm_baseline, fvm_origin, fvm_advanceat, fvm_advanceto, -1 };
	int i, metrics;
	if ( GGadgetGetCid(e->u.control.g)==10 ) {
	    metrics = 0;
	    for ( i=0; masks[i]!=-1 ; ++i )
		if ( GGadgetIsChecked(GWidgetGetControl(gw,masks[i])))
		    metrics |= masks[i];
	    if ( d->ish )
		default_fv_showhmetrics = d->fv->showhmetrics = metrics;
	    else
		default_fv_showvmetrics = d->fv->showvmetrics = metrics;
	}
	d->done = true;
    } else if ( e->type==et_char ) {
return( false );
    }
return( true );
}