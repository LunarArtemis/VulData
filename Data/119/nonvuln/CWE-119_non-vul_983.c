static void GTextField_Replace(GTextField *gt, const unichar_t *str) {
    _GTextFieldReplace(gt,str);
    GTextField_Show(gt,gt->sel_start);
}