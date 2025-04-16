static void GListFieldSelected(GGadget *g, int i) {
    GListField *ge = (GListField *) g;

    ge->popup = NULL;
    _GWidget_ClearGrabGadget(&ge->gt.g);
    if ( i<0 || i>=ge->ltot || ge->ti[i]->text==NULL )
return;
    GTextFieldSetTitle(g,ge->ti[i]->text);
    _ggadget_redraw(g);

    GTextFieldChanged(&ge->gt,i);
}