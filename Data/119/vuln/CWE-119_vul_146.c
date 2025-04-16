static xps_resource *
xps_parse_remote_resource_dictionary(fz_context *ctx, xps_document *doc, char *base_uri, char *source_att)
{
	char part_name[1024];
	char part_uri[1024];
	xps_resource *dict;
	xps_part *part;
	fz_xml *xml = NULL;
	char *s;

	/* External resource dictionaries MUST NOT reference other resource dictionaries */
	xps_resolve_url(ctx, doc, part_name, base_uri, source_att, sizeof part_name);
	part = xps_read_part(ctx, doc, part_name);
	fz_try(ctx)
	{
		xml = fz_parse_xml(ctx, part->data, 0);
	}
	fz_always(ctx)
	{
		xps_drop_part(ctx, doc, part);
	}
	fz_catch(ctx)
	{
		fz_rethrow_if(ctx, FZ_ERROR_TRYLATER);
		xml = NULL;
	}

	if (!xml)
		return NULL;

	if (strcmp(fz_xml_tag(xml), "ResourceDictionary"))
	{
		fz_drop_xml(ctx, xml);
		fz_throw(ctx, FZ_ERROR_GENERIC, "expected ResourceDictionary element");
	}

	fz_strlcpy(part_uri, part_name, sizeof part_uri);
	s = strrchr(part_uri, '/');
	if (s)
		s[1] = 0;

	dict = xps_parse_resource_dictionary(ctx, doc, part_uri, xml);
	if (dict)
		dict->base_xml = xml; /* pass on ownership */
	else
		fz_drop_xml(ctx, xml);

	return dict;
}