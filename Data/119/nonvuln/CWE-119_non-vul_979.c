void GTextFieldReplace(GGadget *g,const unichar_t *txt) {
    GTextField *gt = (GTextField *) g;

    GTextField_Replace(gt,txt);
    _ggadget_redraw(g);
}