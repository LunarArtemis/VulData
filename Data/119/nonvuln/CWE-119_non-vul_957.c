static void GTFPopupMenu(GTextField *gt, GEvent *event) {
    int no_sel = gt->sel_start==gt->sel_end;

    if ( first ) {
	gtf_popuplist[0].ti.text = (unichar_t *) _("_Undo");
	gtf_popuplist[2].ti.text = (unichar_t *) _("Cu_t");
	gtf_popuplist[3].ti.text = (unichar_t *) _("_Copy");
	gtf_popuplist[4].ti.text = (unichar_t *) _("_Paste");
	gtf_popuplist[6].ti.text = (unichar_t *) _("_Save in UTF8");
	gtf_popuplist[7].ti.text = (unichar_t *) _("Save in _UCS2");
	gtf_popuplist[8].ti.text = (unichar_t *) _("_Import");
	first = false;
    }

    gtf_popuplist[0].ti.disabled = gt->oldtext==NULL;	/* Undo */
    gtf_popuplist[2].ti.disabled = no_sel;		/* Cut */
    gtf_popuplist[3].ti.disabled = no_sel;		/* Copy */
    gtf_popuplist[4].ti.disabled = !GDrawSelectionHasType(gt->g.base,sn_clipboard,"text/plain;charset=ISO-10646-UCS-2") &&
	    !GDrawSelectionHasType(gt->g.base,sn_clipboard,"UTF8_STRING") &&
	    !GDrawSelectionHasType(gt->g.base,sn_clipboard,"STRING");
    popup_kludge = gt;
    GMenuCreatePopupMenu(gt->g.base,event, gtf_popuplist);
}