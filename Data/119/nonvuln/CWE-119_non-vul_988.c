static void GTextFieldGrabSelection(GTextField *gt, enum selnames sel ) {

    if ( gt->sel_start!=gt->sel_end ) {
	unichar_t *temp;
	char *ctemp, *ctemp2;
	int i;
	uint16 *u2temp;

	GDrawGrabSelection(gt->g.base,sel);
	temp = malloc((gt->sel_end-gt->sel_start + 2)*sizeof(unichar_t));
	temp[0] = 0xfeff;		/* KDE expects a byte order flag */
	u_strncpy(temp+1,gt->text+gt->sel_start,gt->sel_end-gt->sel_start);
	ctemp = u2utf8_copy(temp+1);
	ctemp2 = u2def_copy(temp+1);
	GDrawAddSelectionType(gt->g.base,sel,"text/plain;charset=ISO-10646-UCS-4",temp,u_strlen(temp),
		sizeof(unichar_t),
		NULL,NULL);
	u2temp = malloc((gt->sel_end-gt->sel_start + 2)*sizeof(uint16));
	for ( i=0; temp[i]!=0; ++i )
	    u2temp[i] = temp[i];
	u2temp[i] = 0;
	GDrawAddSelectionType(gt->g.base,sel,"text/plain;charset=ISO-10646-UCS-2",u2temp,u_strlen(temp),
		2,
		NULL,NULL);
	GDrawAddSelectionType(gt->g.base,sel,"UTF8_STRING",copy(ctemp),strlen(ctemp),
		sizeof(char),
		NULL,NULL);
	GDrawAddSelectionType(gt->g.base,sel,"text/plain;charset=UTF-8",ctemp,strlen(ctemp),
		sizeof(char),
		NULL,NULL);

	if ( ctemp2!=NULL && *ctemp2!='\0' /*strlen(ctemp2)==gt->sel_end-gt->sel_start*/ )
	    GDrawAddSelectionType(gt->g.base,sel,"STRING",ctemp2,strlen(ctemp2),
		    sizeof(char),
		    NULL,NULL);
	else
	    free(ctemp2);
    }
}