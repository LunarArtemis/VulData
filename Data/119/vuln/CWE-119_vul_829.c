pdf_obj *
pdf_parse_ind_obj(fz_context *ctx, pdf_document *doc,
	fz_stream *file, pdf_lexbuf *buf,
	int *onum, int *ogen, int64_t *ostmofs, int *try_repair)
{
	pdf_obj *obj = NULL;
	int num = 0, gen = 0;
	int64_t stm_ofs;
	pdf_token tok;
	int64_t a, b;
	int read_next_token = 1;

	fz_var(obj);

	tok = pdf_lex(ctx, file, buf);
	if (tok != PDF_TOK_INT)
	{
		if (try_repair)
			*try_repair = 1;
		fz_throw(ctx, FZ_ERROR_SYNTAX, "expected object number");
	}
	num = buf->i;

	tok = pdf_lex(ctx, file, buf);
	if (tok != PDF_TOK_INT)
	{
		if (try_repair)
			*try_repair = 1;
		fz_throw(ctx, FZ_ERROR_SYNTAX, "expected generation number (%d ? obj)", num);
	}
	gen = buf->i;

	tok = pdf_lex(ctx, file, buf);
	if (tok != PDF_TOK_OBJ)
	{
		if (try_repair)
			*try_repair = 1;
		fz_throw(ctx, FZ_ERROR_SYNTAX, "expected 'obj' keyword (%d %d ?)", num, gen);
	}

	tok = pdf_lex(ctx, file, buf);

	switch (tok)
	{
	case PDF_TOK_OPEN_ARRAY:
		obj = pdf_parse_array(ctx, doc, file, buf);
		break;

	case PDF_TOK_OPEN_DICT:
		obj = pdf_parse_dict(ctx, doc, file, buf);
		break;

	case PDF_TOK_NAME: obj = pdf_new_name(ctx, doc, buf->scratch); break;
	case PDF_TOK_REAL: obj = pdf_new_real(ctx, doc, buf->f); break;
	case PDF_TOK_STRING: obj = pdf_new_string(ctx, doc, buf->scratch, buf->len); break;
	case PDF_TOK_TRUE: obj = pdf_new_bool(ctx, doc, 1); break;
	case PDF_TOK_FALSE: obj = pdf_new_bool(ctx, doc, 0); break;
	case PDF_TOK_NULL: obj = pdf_new_null(ctx, doc); break;

	case PDF_TOK_INT:
		a = buf->i;
		tok = pdf_lex(ctx, file, buf);

		if (tok == PDF_TOK_STREAM || tok == PDF_TOK_ENDOBJ)
		{
			obj = pdf_new_int(ctx, doc, a);
			read_next_token = 0;
			break;
		}
		else if (tok == PDF_TOK_INT)
		{
			b = buf->i;
			tok = pdf_lex(ctx, file, buf);
			if (tok == PDF_TOK_R)
			{
				obj = pdf_new_indirect(ctx, doc, a, b);
				break;
			}
		}
		fz_throw(ctx, FZ_ERROR_SYNTAX, "expected 'R' keyword (%d %d R)", num, gen);

	case PDF_TOK_ENDOBJ:
		obj = pdf_new_null(ctx, doc);
		read_next_token = 0;
		break;

	default:
		fz_throw(ctx, FZ_ERROR_SYNTAX, "syntax error in object (%d %d R)", num, gen);
	}

	fz_try(ctx)
	{
		if (read_next_token)
			tok = pdf_lex(ctx, file, buf);

		if (tok == PDF_TOK_STREAM)
		{
			int c = fz_read_byte(ctx, file);
			while (c == ' ')
				c = fz_read_byte(ctx, file);
			if (c == '\r')
			{
				c = fz_peek_byte(ctx, file);
				if (c != '\n')
					fz_warn(ctx, "line feed missing after stream begin marker (%d %d R)", num, gen);
				else
					fz_read_byte(ctx, file);
			}
			stm_ofs = fz_tell(ctx, file);
		}
		else if (tok == PDF_TOK_ENDOBJ)
		{
			stm_ofs = 0;
		}
		else
		{
			fz_warn(ctx, "expected 'endobj' or 'stream' keyword (%d %d R)", num, gen);
			stm_ofs = 0;
		}
	}
	fz_catch(ctx)
	{
		pdf_drop_obj(ctx, obj);
		fz_rethrow(ctx);
	}

	if (onum) *onum = num;
	if (ogen) *ogen = gen;
	if (ostmofs) *ostmofs = stm_ofs;

	return obj;
}