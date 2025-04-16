static void GTextFieldGrabDDSelection(GTextField *gt) {

    GDrawGrabSelection(gt->g.base,sn_drag_and_drop);
    GDrawAddSelectionType(gt->g.base,sn_drag_and_drop,"text/plain;charset=ISO-10646-UCS-4",gt,gt->sel_end-gt->sel_start,
	    sizeof(unichar_t),
	    ddgenunicodedata,noop);
    GDrawAddSelectionType(gt->g.base,sn_drag_and_drop,"STRING",gt,gt->sel_end-gt->sel_start,sizeof(char),
	    ddgenlocaldata,noop);
}