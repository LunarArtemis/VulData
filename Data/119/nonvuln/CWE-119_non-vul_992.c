static void GTextFieldDrawLineSel(GWindow pixmap, GTextField *gt, int line ) {
    GRect selr, sofar, nextch;
    int s,e, y,llen,i,j;

    /* Caller has checked to make sure selection applies to this line */

    y = gt->g.inner.y+(line-gt->loff_top)*gt->fh;
    selr = gt->g.inner; selr.y = y; selr.height = gt->fh;
    if ( !gt->g.has_focus ) --selr.height;
    llen = gt->lines[line+1]==-1?
	    u_strlen(gt->text+gt->lines[line])+gt->lines[line]:
	    gt->lines[line+1];
    s = gt->sel_start<gt->lines[line]?gt->lines[line]:gt->sel_start;
    e = gt->sel_end>gt->lines[line+1] && gt->lines[line+1]!=-1?gt->lines[line+1]-1:
	    gt->sel_end;

    s = u2utf8_index(s-gt->lines[line],gt->utf8_text+gt->lines8[line]);
    e = u2utf8_index(e-gt->lines[line],gt->utf8_text+gt->lines8[line]);
    llen = gt->lines8[line+1]==-1? -1 : gt->lines8[line+1]-gt->lines8[line];
    GDrawLayoutInit(pixmap,gt->utf8_text+gt->lines8[line],llen,NULL);
    for ( i=s; i<e; ) {
        GDrawLayoutIndexToPos(pixmap,i,&sofar);
        for ( j=i+1; j<e; ++j ) {
	    GDrawLayoutIndexToPos(pixmap,j,&nextch);
	    if ( nextch.x != sofar.x+sofar.width )
        break;
	    sofar.width += nextch.width;
        }
        if ( sofar.width<0 ) {
	    selr.x = sofar.x+sofar.width + gt->g.inner.x - gt->xoff_left;
	    selr.width = -sofar.width;
        } else {
	    selr.x = sofar.x + gt->g.inner.x - gt->xoff_left;
	    selr.width = sofar.width;
        }
        GDrawFillRect(pixmap,&selr,gt->g.box->active_border);
        i = j;
    }
}