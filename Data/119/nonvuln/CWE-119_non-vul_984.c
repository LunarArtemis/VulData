static int GTextField_Show(GTextField *gt, int pos) {
    int i, ll, xoff, loff;
    int refresh=false;
    GListField *ge = (GListField *) gt;
    int width = gt->g.inner.width;

    if ( gt->listfield || gt->numericfield )
	width = ge->fieldrect.width - 2*(gt->g.inner.x - gt->g.r.x);

    if ( pos < 0 ) pos = 0;
    if ( pos > u_strlen(gt->text)) pos = u_strlen(gt->text);
    i = GTextFieldFindLine(gt,pos);

    loff = gt->loff_top;
    if ( gt->lcnt<gt->g.inner.height/gt->fh || loff==0 )
	loff = 0;
    if ( i<loff )
	loff = i;
    if ( i>=loff+gt->g.inner.height/gt->fh ) {
	loff = i-(gt->g.inner.height/gt->fh);
	if ( gt->g.inner.height/gt->fh>2 )
	    ++loff;
    }

    xoff = gt->xoff_left;
    if ( gt->lines[i+1]==-1 ) ll = -1; else ll = gt->lines[i+1]-gt->lines[i]-1;
    GRect size;
    if ( gt->lines8[i+1]==-1 ) ll = strlen(gt->utf8_text+gt->lines8[i]); else ll = gt->lines8[i+1]-gt->lines8[i]-1;
    GDrawLayoutInit(gt->g.base,gt->utf8_text+gt->lines8[i],ll,NULL);
    GDrawLayoutExtents(gt->g.base,&size);
    if ( size.width < width )
	    xoff = 0;
    else {
	int index8 = u2utf8_index(pos- gt->lines8[i],gt->utf8_text + gt->lines8[i]);
	GDrawLayoutIndexToPos(gt->g.base,index8,&size);
	if ( size.x + 2*size.width < width )
	    xoff = 0;
	else
	    xoff = size.x - (width - size.width)/2;
	if ( xoff<0 )
	    xoff = 0;
    }

    if ( xoff!=gt->xoff_left ) {
	gt->xoff_left = xoff;
	if ( gt->hsb!=NULL )
	    GScrollBarSetPos(&gt->hsb->g,xoff);
	refresh = true;
    }
    if ( loff!=gt->loff_top ) {
	gt->loff_top = loff;
	if ( gt->vsb!=NULL )
	    GScrollBarSetPos(&gt->vsb->g,loff);
	refresh = true;
    }
    GTPositionGIC(gt);
return( refresh );
}