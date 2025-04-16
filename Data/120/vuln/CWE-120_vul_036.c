static pjmedia_sdp_attr *parse_attr( pj_pool_t *pool, pj_scanner *scanner,
				    volatile parse_context *ctx)
{
    pjmedia_sdp_attr *attr;

    ctx->last_error = PJMEDIA_SDP_EINATTR;

    attr = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_attr);

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
	on_scanner_error(scanner);
	return NULL;
    }

    /* skip a= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);
    
    /* get attr name. */
    pj_scan_get(scanner, &cs_token, &attr->name);

    if (*scanner->curptr && *scanner->curptr != '\r' && 
	*scanner->curptr != '\n') 
    {
	/* skip ':' if present. */
	if (*scanner->curptr == ':')
	    pj_scan_get_char(scanner);

	/* get value */
	if (*scanner->curptr != '\r' && *scanner->curptr != '\n') {
	    pj_scan_get_until_chr(scanner, "\r\n", &attr->value);
	} else {
	    attr->value.ptr = NULL;
	    attr->value.slen = 0;
	}

    } else {
	attr->value.ptr = NULL;
	attr->value.slen = 0;
    }

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);

    return attr;
}