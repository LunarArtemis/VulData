static void parse_time(pj_scanner *scanner, pjmedia_sdp_session *ses,
		       volatile parse_context *ctx)
{
    pj_str_t str;

    ctx->last_error = PJMEDIA_SDP_EINTIME;

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
	on_scanner_error(scanner);
	return;
    }

    /* t= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* start time */
    pj_scan_get_until_ch(scanner, ' ', &str);
    ses->time.start = pj_strtoul(&str);

    pj_scan_get_char(scanner);

    /* stop time */
    pj_scan_get_until_chr(scanner, " \t\r\n", &str);
    ses->time.stop = pj_strtoul(&str);

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}