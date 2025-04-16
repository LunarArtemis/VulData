static void
svg_run_use(fz_context *ctx, fz_device *dev, svg_document *doc, fz_xml *root, const svg_state *inherit_state)
{
	svg_state local_state = *inherit_state;

	char *xlink_href_att = fz_xml_att(root, "xlink:href");
	char *x_att = fz_xml_att(root, "x");
	char *y_att = fz_xml_att(root, "y");

	float x = 0;
	float y = 0;

	svg_parse_common(ctx, doc, root, &local_state);
	if (x_att) x = svg_parse_length(x_att, local_state.viewbox_w, local_state.fontsize);
	if (y_att) y = svg_parse_length(y_att, local_state.viewbox_h, local_state.fontsize);

	fz_pre_translate(&local_state.transform, x, y);

	if (xlink_href_att && xlink_href_att[0] == '#')
	{
		fz_xml *linked = fz_tree_lookup(ctx, doc->idmap, xlink_href_att + 1);
		if (linked)
		{
			if (!strcmp(fz_xml_tag(linked), "symbol"))
				svg_run_use_symbol(ctx, dev, doc, root, linked, &local_state);
			else
				svg_run_element(ctx, dev, doc, linked, &local_state);
			return;
		}
	}

	fz_warn(ctx, "svg: cannot find linked symbol");
}