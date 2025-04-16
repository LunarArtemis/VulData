static void GTextFieldImport(GTextField *gt) {
    unichar_t *ret;
    char *cret;
    unichar_t *str;

    if ( _ggadget_use_gettext ) {
	char *temp = GWidgetOpenFile8(_("Open"),NULL,"*.{txt,py}",NULL,NULL);
	ret = utf82u_copy(temp);
	free(temp);
    } else {
	ret = GWidgetOpenFile(GStringGetResource(_STR_Open,NULL),NULL,
		txt,NULL,NULL);
    }

    if ( ret==NULL )
return;
    cret = u2def_copy(ret);
    free(ret);
    str = _GGadgetFileToUString(cret,65536);
    if ( str==NULL ) {
	if ( _ggadget_use_gettext )
	    GWidgetError8(_("Could not open file"), _("Could not open %.100s"),cret);
	else
	    GWidgetError(errort,error,cret);
	free(cret);
return;
    }
    free(cret);
    GTextField_Replace(gt,str);
    free(str);
}