static void GTextFieldGrabPrimarySelection(GTextField *gt) {
    int ss = gt->sel_start, se = gt->sel_end;

    GDrawGrabSelection(gt->g.base,sn_primary);
    gt->sel_start = ss; gt->sel_end = se;
    GDrawAddSelectionType(gt->g.base,sn_primary,"text/plain;charset=ISO-10646-UCS-4",gt,gt->sel_end-gt->sel_start,
	    sizeof(unichar_t),
	    genunicodedata,noop);
    GDrawAddSelectionType(gt->g.base,sn_primary,"UTF8_STRING",gt,gt->sel_end-gt->sel_start,
	    sizeof(char),
	    genutf8data,noop);
    GDrawAddSelectionType(gt->g.base,sn_primary,"text/plain;charset=UTF-8",gt,gt->sel_end-gt->sel_start,
	    sizeof(char),
	    genutf8data,noop);
    GDrawAddSelectionType(gt->g.base,sn_primary,"STRING",gt,gt->sel_end-gt->sel_start,
	    sizeof(char),
	    genlocaldata,noop);
}