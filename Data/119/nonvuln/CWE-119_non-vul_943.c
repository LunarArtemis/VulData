static int gtextfield_FillsWindow(GGadget *g) {
return( ((GTextField *) g)->multi_line && g->prev==NULL &&
	(_GWidgetGetGadgets(g->base)==g ||
	 _GWidgetGetGadgets(g->base)==(GGadget *) ((GTextField *) g)->vsb ||
	 _GWidgetGetGadgets(g->base)==(GGadget *) ((GTextField *) g)->hsb ));
}