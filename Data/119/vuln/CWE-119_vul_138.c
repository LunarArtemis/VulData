static void
xps_load_links_in_glyphs(fz_context *ctx, xps_document *doc, const fz_matrix *ctm,
		char *base_uri, xps_resource *dict, fz_xml *root, fz_link **link)
{
	char *navigate_uri_att = fz_xml_att(root, "FixedPage.NavigateUri");
	if (navigate_uri_att)
	{
		char *transform_att = fz_xml_att(root, "RenderTransform");
		fz_xml *transform_tag = fz_xml_down(fz_xml_find_down(root, "Path.RenderTransform"));

		char *bidi_level_att = fz_xml_att(root, "BidiLevel");
		char *font_size_att = fz_xml_att(root, "FontRenderingEmSize");
		char *font_uri_att = fz_xml_att(root, "FontUri");
		char *origin_x_att = fz_xml_att(root, "OriginX");
		char *origin_y_att = fz_xml_att(root, "OriginY");
		char *is_sideways_att = fz_xml_att(root, "IsSideways");
		char *indices_att = fz_xml_att(root, "Indices");
		char *unicode_att = fz_xml_att(root, "UnicodeString");
		char *style_att = fz_xml_att(root, "StyleSimulations");

		int is_sideways = 0;
		int bidi_level = 0;
		fz_matrix local_ctm;
		fz_font *font;
		fz_text *text;
		fz_rect area;

		xps_resolve_resource_reference(ctx, doc, dict, &transform_att, &transform_tag, NULL);

		xps_parse_transform(ctx, doc, transform_att, transform_tag, &local_ctm, ctm);

		if (is_sideways_att)
			is_sideways = !strcmp(is_sideways_att, "true");
		if (bidi_level_att)
			bidi_level = atoi(bidi_level_att);

		font = xps_lookup_font(ctx, doc, base_uri, font_uri_att, style_att);
		text = xps_parse_glyphs_imp(ctx, doc, &local_ctm, font, fz_atof(font_size_att),
				fz_atof(origin_x_att), fz_atof(origin_y_att),
				is_sideways, bidi_level, indices_att, unicode_att);
		fz_bound_text(ctx, text, NULL, &local_ctm, &area);
		fz_drop_text(ctx, text);
		fz_drop_font(ctx, font);

		xps_add_link(ctx, doc, &area, base_uri, navigate_uri_att, link);
	}
}