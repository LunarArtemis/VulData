void
xps_parse_glyphs(fz_context *ctx, xps_document *doc, const fz_matrix *ctm,
		char *base_uri, xps_resource *dict, fz_xml *root)
{
	fz_device *dev = doc->dev;

	fz_xml *node;

	char *fill_uri;
	char *opacity_mask_uri;

	char *bidi_level_att;
	char *fill_att;
	char *font_size_att;
	char *font_uri_att;
	char *origin_x_att;
	char *origin_y_att;
	char *is_sideways_att;
	char *indices_att;
	char *unicode_att;
	char *style_att;
	char *transform_att;
	char *clip_att;
	char *opacity_att;
	char *opacity_mask_att;

	fz_xml *transform_tag = NULL;
	fz_xml *clip_tag = NULL;
	fz_xml *fill_tag = NULL;
	fz_xml *opacity_mask_tag = NULL;

	char *fill_opacity_att = NULL;

	fz_font *font;

	float font_size = 10;
	int is_sideways = 0;
	int bidi_level = 0;

	fz_text *text;
	fz_rect area;

	fz_matrix local_ctm;

	/*
	 * Extract attributes and extended attributes.
	 */

	bidi_level_att = fz_xml_att(root, "BidiLevel");
	fill_att = fz_xml_att(root, "Fill");
	font_size_att = fz_xml_att(root, "FontRenderingEmSize");
	font_uri_att = fz_xml_att(root, "FontUri");
	origin_x_att = fz_xml_att(root, "OriginX");
	origin_y_att = fz_xml_att(root, "OriginY");
	is_sideways_att = fz_xml_att(root, "IsSideways");
	indices_att = fz_xml_att(root, "Indices");
	unicode_att = fz_xml_att(root, "UnicodeString");
	style_att = fz_xml_att(root, "StyleSimulations");
	transform_att = fz_xml_att(root, "RenderTransform");
	clip_att = fz_xml_att(root, "Clip");
	opacity_att = fz_xml_att(root, "Opacity");
	opacity_mask_att = fz_xml_att(root, "OpacityMask");

	for (node = fz_xml_down(root); node; node = fz_xml_next(node))
	{
		if (fz_xml_is_tag(node, "Glyphs.RenderTransform"))
			transform_tag = fz_xml_down(node);
		if (fz_xml_is_tag(node, "Glyphs.OpacityMask"))
			opacity_mask_tag = fz_xml_down(node);
		if (fz_xml_is_tag(node, "Glyphs.Clip"))
			clip_tag = fz_xml_down(node);
		if (fz_xml_is_tag(node, "Glyphs.Fill"))
			fill_tag = fz_xml_down(node);
	}

	fill_uri = base_uri;
	opacity_mask_uri = base_uri;

	xps_resolve_resource_reference(ctx, doc, dict, &transform_att, &transform_tag, NULL);
	xps_resolve_resource_reference(ctx, doc, dict, &clip_att, &clip_tag, NULL);
	xps_resolve_resource_reference(ctx, doc, dict, &fill_att, &fill_tag, &fill_uri);
	xps_resolve_resource_reference(ctx, doc, dict, &opacity_mask_att, &opacity_mask_tag, &opacity_mask_uri);

	/*
	 * Check that we have all the necessary information.
	 */

	if (!font_size_att || !font_uri_att || !origin_x_att || !origin_y_att) {
		fz_warn(ctx, "missing attributes in glyphs element");
		return;
	}

	if (!indices_att && !unicode_att)
		return; /* nothing to draw */

	if (is_sideways_att)
		is_sideways = !strcmp(is_sideways_att, "true");

	if (bidi_level_att)
		bidi_level = atoi(bidi_level_att);

	/*
	 * Find and load the font resource.
	 */

	font = xps_lookup_font(ctx, doc, base_uri, font_uri_att, style_att);
	if (!font)
		return; /* bail if we can't find the font */

	/*
	 * Set up graphics state.
	 */

	xps_parse_transform(ctx, doc, transform_att, transform_tag, &local_ctm, ctm);

	if (clip_att || clip_tag)
		xps_clip(ctx, doc, &local_ctm, dict, clip_att, clip_tag);

	font_size = fz_atof(font_size_att);

	text = xps_parse_glyphs_imp(ctx, doc, &local_ctm, font, font_size,
			fz_atof(origin_x_att), fz_atof(origin_y_att),
			is_sideways, bidi_level, indices_att, unicode_att);

	fz_bound_text(ctx, text, NULL, &local_ctm, &area);

	xps_begin_opacity(ctx, doc, &local_ctm, &area, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);

	/* If it's a solid color brush fill/stroke do a simple fill */

	if (fill_tag && !strcmp(fz_xml_tag(fill_tag), "SolidColorBrush"))
	{
		fill_opacity_att = fz_xml_att(fill_tag, "Opacity");
		fill_att = fz_xml_att(fill_tag, "Color");
		fill_tag = NULL;
	}

	if (fill_att)
	{
		float samples[FZ_MAX_COLORS];
		fz_colorspace *colorspace;

		xps_parse_color(ctx, doc, base_uri, fill_att, &colorspace, samples);
		if (fill_opacity_att)
			samples[0] *= fz_atof(fill_opacity_att);
		xps_set_color(ctx, doc, colorspace, samples);

		fz_fill_text(ctx, dev, text, &local_ctm,
			doc->colorspace, doc->color, doc->alpha, NULL);
	}

	/* If it's a complex brush, use the charpath as a clip mask */

	if (fill_tag)
	{
		fz_clip_text(ctx, dev, text, &local_ctm, &area);
		xps_parse_brush(ctx, doc, &local_ctm, &area, fill_uri, dict, fill_tag);
		fz_pop_clip(ctx, dev);
	}

	xps_end_opacity(ctx, doc, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);

	fz_drop_text(ctx, text);

	if (clip_att || clip_tag)
		fz_pop_clip(ctx, dev);

	fz_drop_font(ctx, font);
}