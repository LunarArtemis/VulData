static void FontViewSetTitle(FontView *fv) {
    unichar_t *title, *ititle, *temp;
    char *file=NULL;
    char *enc;
    int len;

    if ( fv->gw==NULL )		/* In scripting */
return;

    enc = SFEncodingName(fv->b.sf,fv->b.normal?fv->b.normal:fv->b.map);
    len = strlen(fv->b.sf->fontname)+1 + strlen(enc)+6;
    if ( fv->b.normal ) len += strlen(_("Compact"))+1;
    if ( fv->b.cidmaster!=NULL ) {
	if ( (file = fv->b.cidmaster->filename)==NULL )
	    file = fv->b.cidmaster->origname;
    } else {
	if ( (file = fv->b.sf->filename)==NULL )
	    file = fv->b.sf->origname;
    }
    if ( file!=NULL )
	len += 2+strlen(file);
    title = malloc((len+1)*sizeof(unichar_t));
    uc_strcpy(title,"");
    uc_strcat(title,fv->b.sf->fontname);
    if ( fv->b.sf->changed )
	uc_strcat(title,"*");
    if ( file!=NULL ) {
	uc_strcat(title,"  ");
	temp = def2u_copy(GFileNameTail(file));
	u_strcat(title,temp);
	free(temp);
    }
    uc_strcat(title, " (" );
    if ( fv->b.normal ) { utf82u_strcat(title,_("Compact")); uc_strcat(title," "); }
    uc_strcat(title,enc);
    uc_strcat(title, ")" );
    free(enc);

    ititle = uc_copy(fv->b.sf->fontname);
    GDrawSetWindowTitles(fv->gw,title,ititle);
    free(title);
    free(ititle);
}