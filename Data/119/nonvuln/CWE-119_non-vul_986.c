static void GTextFieldPangoRefigureLines(GTextField *gt, int start_of_change) {
    char *utf8_text, *pt, *ept;
    unichar_t *upt, *uept;
    int i, uc;
    GRect size;

    free(gt->utf8_text);
    if ( gt->lines8==NULL ) {
	gt->lines8 = malloc(gt->lmax*sizeof(int32));
	gt->lines8[0] = 0;
	gt->lines8[1] = -1;
    }

    if ( gt->password ) {
	int cnt = u_strlen(gt->text);
	utf8_text = malloc(cnt+1);
	memset(utf8_text,'*',cnt);
	utf8_text[cnt] = '\0';
    } else
	utf8_text = u2utf8_copy(gt->text);
    gt->utf8_text = utf8_text;
    GDrawLayoutInit(gt->g.base,utf8_text,-1,NULL);

    if ( !gt->multi_line ) {
	GDrawLayoutExtents(gt->g.base,&size);
	gt->xmax = size.width;
return;
    }

    if ( !gt->wrap ) {
	pt = utf8_text;
	i=0;
	while ( ( ept = strchr(pt,'\n'))!=NULL ) {
	    if ( i>=gt->lmax ) {
		gt->lines8 = realloc(gt->lines8,(gt->lmax+=10)*sizeof(int32));
		gt->lines = realloc(gt->lines,gt->lmax*sizeof(int32));
	    }
	    gt->lines8[i++] = pt-utf8_text;
	    pt = ept+1;
	}
	if ( i>=gt->lmax ) {
	    gt->lines8 = realloc(gt->lines8,(gt->lmax+=10)*sizeof(int32));
	    gt->lines = realloc(gt->lines,gt->lmax*sizeof(int32));
	}
	gt->lines8[i++] = pt-utf8_text;

	upt = gt->text;
	i = 0;
	while ( ( uept = u_strchr(upt,'\n'))!=NULL ) {
	    gt->lines[i++] = upt-gt->text;
	    upt = uept+1;
	}
	gt->lines[i++] = upt-gt->text;
    } else {
	int lcnt;
	GDrawLayoutSetWidth(gt->g.base,gt->g.inner.width);
	lcnt = GDrawLayoutLineCount(gt->g.base);
	if ( lcnt+2>=gt->lmax ) {
	    gt->lines8 = realloc(gt->lines8,(gt->lmax=lcnt+10)*sizeof(int32));
	    gt->lines = realloc(gt->lines,gt->lmax*sizeof(int32));
	}
	pt = utf8_text; uc=0;
	for ( i=0; i<lcnt; ++i ) {
	    gt->lines8[i] = GDrawLayoutLineStart(gt->g.base,i);
	    ept = utf8_text + gt->lines8[i];
	    while ( pt<ept ) {
		++uc;
		utf8_ildb((const char **) &pt);
	    }
	    gt->lines[i] = uc;
	}
	if ( i==0 ) {
	    gt->lines8[i] = strlen(utf8_text);
	    gt->lines[i] = u_strlen(gt->text);
	} else {
	    gt->lines8[i] = gt->lines8[i-1] +   strlen( utf8_text + gt->lines8[i-1]);
	    gt->lines [i] = gt->lines [i-1] + u_strlen(  gt->text + gt->lines [i-1]);
	}
    }
    if ( gt->lcnt!=i ) {
	gt->lcnt = i;
	if ( gt->vsb!=NULL )
	    GScrollBarSetBounds(&gt->vsb->g,0,gt->lcnt,
		    gt->g.inner.height<gt->fh? 1 : gt->g.inner.height/gt->fh);
	if ( gt->loff_top+gt->g.inner.height/gt->fh>gt->lcnt ) {
	    gt->loff_top = gt->lcnt-gt->g.inner.height/gt->fh;
	    if ( gt->loff_top<0 ) gt->loff_top = 0;
	    if ( gt->vsb!=NULL )
		GScrollBarSetPos(&gt->vsb->g,gt->loff_top);
	}
    }
    if ( i>=gt->lmax )
	gt->lines = realloc(gt->lines,(gt->lmax+=10)*sizeof(int32));
    gt->lines8[i] = -1;
    gt->lines[i++] = -1;

    GDrawLayoutExtents(gt->g.base,&size);
    gt->xmax = size.width;

    if ( gt->hsb!=NULL ) {
	GScrollBarSetBounds(&gt->hsb->g,0,gt->xmax,gt->g.inner.width);
    }
    GDrawLayoutSetWidth(gt->g.base,-1);
}