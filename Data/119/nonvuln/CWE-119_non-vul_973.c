static void GTextFieldSetFont(GGadget *g,FontInstance *new) {
    GTextField *gt = (GTextField *) g;
    gt->font = new;
    GTextFieldRefigureLines(gt,0);
}