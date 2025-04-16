static void parse_generic_line(pj_scanner *scanner, pj_str_t *str,
			       volatile parse_context *ctx)
{
    ctx->last_error = PJMEDIA_SDP_EINSDP;

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
	on_scanner_error(scanner);
	return;
    }

    /* x= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* get anything until newline (including whitespaces). */
    pj_scan_get_until_chr(scanner, "\r\n", str);

    /* newline. */
    pj_scan_get_newline(scanner);
}