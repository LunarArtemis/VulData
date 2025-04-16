pdf_xref_entry *pdf_get_populating_xref_entry(fz_context *ctx, pdf_document *doc, int num)
{
	/* Return an entry within the xref currently being populated */
	pdf_xref *xref;
	pdf_xref_subsec *sub;

	if (doc->num_xref_sections == 0)
	{
		doc->xref_sections = fz_malloc_struct(ctx, pdf_xref);
		doc->num_xref_sections = 1;
	}

	/* Prevent accidental heap underflow */
	if (num < 0)
		fz_throw(ctx, FZ_ERROR_GENERIC, "object number must not be negative (%d)", num);

	/* Return the pointer to the entry in the last section. */
	xref = &doc->xref_sections[doc->num_xref_sections-1];

	for (sub = xref->subsec; sub != NULL; sub = sub->next)
	{
		if (num >= sub->start && num < sub->start + sub->len)
			return &sub->table[num-sub->start];
	}

	/* We've been asked for an object that's not in a subsec. */
	ensure_solid_xref(ctx, doc, num+1, doc->num_xref_sections-1);
	xref = &doc->xref_sections[doc->num_xref_sections-1];
	sub = xref->subsec;

	return &sub->table[num-sub->start];
}