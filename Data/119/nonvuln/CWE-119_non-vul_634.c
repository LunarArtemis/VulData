static int SS_ScriptChanged(GGadget *g, GEvent *e) {

    if ( e->type==et_controlevent && e->u.control.subtype != et_textfocuschanged ) {
	char *txt = GGadgetGetTitle8(g);
	char buf[8];
	int i;
	extern GTextInfo scripts[];

	for ( i=0; scripts[i].text!=NULL; ++i ) {
	    if ( strcmp((char *) scripts[i].text,txt)==0 )
	break;
	}
	free(txt);
	if ( scripts[i].text==NULL )
return( true );
	buf[0] = ((intpt) scripts[i].userdata)>>24;
	buf[1] = ((intpt) scripts[i].userdata)>>16;
	buf[2] = ((intpt) scripts[i].userdata)>>8 ;
	buf[3] = ((intpt) scripts[i].userdata)    ;
	buf[4] = 0;
	GGadgetSetTitle8(g,buf);
    }
return( true );
}