static void GTextFieldSetTitle(GGadget *g,const unichar_t *tit) {
    GTextField *gt = (GTextField *) g;
    unichar_t *old = gt->oldtext;
    if ( tit==NULL || u_strcmp(tit,gt->text)==0 )	/* If it doesn't change anything, then don't trash undoes or selection */
return;
    gt->oldtext = gt->text;
    gt->sel_oldstart = gt->sel_start; gt->sel_oldend = gt->sel_end; gt->sel_oldbase = gt->sel_base;
    gt->text = u_copy(tit);		/* tit might be oldtext, so must copy before freeing */
    free(old);
    free(gt->utf8_text);
    gt->utf8_text = u2utf8_copy(gt->text);
    gt->sel_start = gt->sel_end = gt->sel_base = u_strlen(tit);
    GTextFieldRefigureLines(gt,0);
    GTextField_Show(gt,gt->sel_start);
    _ggadget_redraw(g);
}