static void parse_media(pj_scanner *scanner, pjmedia_sdp_media *med,
			volatile parse_context *ctx)
{
    pj_str_t str;

    ctx->last_error = PJMEDIA_SDP_EINMEDIA;

    /* check the equal sign */
    if (*(scanner->curptr+1) != '=') {
	on_scanner_error(scanner);
	return;
    }

    /* m= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* type */
    pj_scan_get_until_ch(scanner, ' ', &med->desc.media);
    pj_scan_get_char(scanner);

    /* port */
    pj_scan_get(scanner, &cs_token, &str);
    med->desc.port = (unsigned short)pj_strtoul(&str);
    if (*scanner->curptr == '/') {
	/* port count */
	pj_scan_get_char(scanner);
	pj_scan_get(scanner, &cs_token, &str);
	med->desc.port_count = pj_strtoul(&str);

    } else {
	med->desc.port_count = 0;
    }

    if (pj_scan_get_char(scanner) != ' ') {
	PJ_THROW(SYNTAX_ERROR);
    }

    /* transport */
    pj_scan_get_until_chr(scanner, " \t\r\n", &med->desc.transport);

    /* format list */
    med->desc.fmt_count = 0;
    while (*scanner->curptr == ' ') {
	pj_str_t fmt;

	pj_scan_get_char(scanner);

	/* Check again for the end of the line */
	if ((*scanner->curptr == '\r') || (*scanner->curptr == '\n'))
		break;

	pj_scan_get(scanner, &cs_token, &fmt);
	if (med->desc.fmt_count < PJMEDIA_MAX_SDP_FMT)
	    med->desc.fmt[med->desc.fmt_count++] = fmt;
	else
	    PJ_PERROR(2,(THIS_FILE, PJ_ETOOMANY, 
		         "Error adding SDP media format %.*s, "
			 "format is ignored",
			 (int)fmt.slen, fmt.ptr));
    }

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}