static void parse_origin(pj_scanner *scanner, pjmedia_sdp_session *ses,
			 volatile parse_context *ctx)
{
    pj_str_t str;

    ctx->last_error = PJMEDIA_SDP_EINORIGIN;

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
	on_scanner_error(scanner);
	return;
    }

    /* o= */
    pj_scan_advance_n(scanner, 2, SKIP_WS);

    /* username. */
    pj_scan_get_until_ch(scanner, ' ', &ses->origin.user);
    pj_scan_get_char(scanner);

    /* id */
    pj_scan_get_until_ch(scanner, ' ', &str);
    ses->origin.id = pj_strtoul(&str);
    pj_scan_get_char(scanner);

    /* version */
    pj_scan_get_until_ch(scanner, ' ', &str);
    ses->origin.version = pj_strtoul(&str);
    pj_scan_get_char(scanner);

    /* network-type */
    pj_scan_get_until_ch(scanner, ' ', &ses->origin.net_type);
    pj_scan_get_char(scanner);

    /* addr-type */
    pj_scan_get_until_ch(scanner, ' ', &ses->origin.addr_type);
    pj_scan_get_char(scanner);

    /* address */
    pj_scan_get_until_chr(scanner, " \t\r\n", &ses->origin.addr);

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);

}