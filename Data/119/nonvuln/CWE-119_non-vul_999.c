static void GTFPopupInvoked(GWindow v, GMenuItem *mi,GEvent *e) {
    GTextField *gt;
    if ( popup_kludge==NULL )
return;
    gt = popup_kludge;
    popup_kludge = NULL;
    switch ( mi->mid ) {
      case MID_Undo:
	gtextfield_editcmd(&gt->g,ec_undo);
      break;
      case MID_Cut:
	gtextfield_editcmd(&gt->g,ec_cut);
      break;
      case MID_Copy:
	gtextfield_editcmd(&gt->g,ec_copy);
      break;
      case MID_Paste:
	gtextfield_editcmd(&gt->g,ec_paste);
      break;
      case MID_SelectAll:
	gtextfield_editcmd(&gt->g,ec_selectall);
      break;
      case MID_Save:
	GTextFieldSave(gt,true);
      break;
      case MID_SaveUCS2:
	GTextFieldSave(gt,false);
      break;
      case MID_Import:
	GTextFieldImport(gt);
      break;
    }
    _ggadget_redraw(&gt->g);
}