void GTextFieldShow(GGadget *g,int pos) {
    GTextField *gt = (GTextField *) g;

    GTextField_Show(gt,pos);
    _ggadget_redraw(g);
}