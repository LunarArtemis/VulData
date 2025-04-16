static int SaveAs_FormatChange(GGadget *g, GEvent *e) {
    if ( e->type==et_controlevent && e->u.control.subtype == et_radiochanged ) {
	GGadget *fc = GWidgetGetControl(GGadgetGetWindow(g),1000);
	char *oldname = GGadgetGetTitle8(fc);
	int *_s2d = GGadgetGetUserData(g);
	int s2d = GGadgetIsChecked(g);
	char *pt, *newname = malloc(strlen(oldname)+8);
	strcpy(newname,oldname);
	pt = strrchr(newname,'.');
	if ( pt==NULL )
	    pt = newname+strlen(newname);
	strcpy(pt,s2d ? ".sfdir" : ".sfd" );
	GGadgetSetTitle8(fc,newname);
	save_to_dir = *_s2d = s2d;
	SavePrefs(true);
    }
return( true );
}