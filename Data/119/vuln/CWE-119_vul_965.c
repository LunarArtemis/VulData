static void
pdf_load_mesh_params(fz_context *ctx, pdf_document *doc, fz_shade *shade, pdf_obj *dict)
{
	pdf_obj *obj;
	int i, n;

	shade->u.m.x0 = shade->u.m.y0 = 0;
	shade->u.m.x1 = shade->u.m.y1 = 1;
	for (i = 0; i < FZ_MAX_COLORS; i++)
	{
		shade->u.m.c0[i] = 0;
		shade->u.m.c1[i] = 1;
	}

	shade->u.m.vprow = pdf_to_int(ctx, pdf_dict_get(ctx, dict, PDF_NAME_VerticesPerRow));
	shade->u.m.bpflag = pdf_to_int(ctx, pdf_dict_get(ctx, dict, PDF_NAME_BitsPerFlag));
	shade->u.m.bpcoord = pdf_to_int(ctx, pdf_dict_get(ctx, dict, PDF_NAME_BitsPerCoordinate));
	shade->u.m.bpcomp = pdf_to_int(ctx, pdf_dict_get(ctx, dict, PDF_NAME_BitsPerComponent));

	obj = pdf_dict_get(ctx, dict, PDF_NAME_Decode);
	if (pdf_array_len(ctx, obj) >= 6)
	{
		n = (pdf_array_len(ctx, obj) - 4) / 2;
		shade->u.m.x0 = pdf_to_real(ctx, pdf_array_get(ctx, obj, 0));
		shade->u.m.x1 = pdf_to_real(ctx, pdf_array_get(ctx, obj, 1));
		shade->u.m.y0 = pdf_to_real(ctx, pdf_array_get(ctx, obj, 2));
		shade->u.m.y1 = pdf_to_real(ctx, pdf_array_get(ctx, obj, 3));
		for (i = 0; i < n; i++)
		{
			shade->u.m.c0[i] = pdf_to_real(ctx, pdf_array_get(ctx, obj, 4 + i * 2));
			shade->u.m.c1[i] = pdf_to_real(ctx, pdf_array_get(ctx, obj, 5 + i * 2));
		}
	}

	if (shade->u.m.vprow < 2 && shade->type == 5)
	{
		fz_warn(ctx, "Too few vertices per row (%d)", shade->u.m.vprow);
		shade->u.m.vprow = 2;
	}

	if (shade->u.m.bpflag != 2 && shade->u.m.bpflag != 4 && shade->u.m.bpflag != 8 &&
		shade->type != 5)
	{
		fz_warn(ctx, "Invalid number of bits per flag (%d)", shade->u.m.bpflag);
		shade->u.m.bpflag = 8;
	}

	if (shade->u.m.bpcoord != 1 && shade->u.m.bpcoord != 2 && shade->u.m.bpcoord != 4 &&
		shade->u.m.bpcoord != 8 && shade->u.m.bpcoord != 12 && shade->u.m.bpcoord != 16 &&
		shade->u.m.bpcoord != 24 && shade->u.m.bpcoord != 32)
	{
		fz_warn(ctx, "Invalid number of bits per coordinate (%d)", shade->u.m.bpcoord);
		shade->u.m.bpcoord = 8;
	}

	if (shade->u.m.bpcomp != 1 && shade->u.m.bpcomp != 2 && shade->u.m.bpcomp != 4 &&
		shade->u.m.bpcomp != 8 && shade->u.m.bpcomp != 12 && shade->u.m.bpcomp != 16)
	{
		fz_warn(ctx, "Invalid number of bits per component (%d)", shade->u.m.bpcomp);
		shade->u.m.bpcomp = 8;
	}
}