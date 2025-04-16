static void parse_version(pj_scanner *scanner, 
                          volatile parse_context *ctx)
{
    ctx->last_error = PJMEDIA_SDP_EINVER;

    /* check equal sign */
    if (*(scanner->curptr+1) != '=') {
	on_scanner_error(scanner);
	return;
    }

    /* check version is 0 */
    if (*(scanner->curptr+2) != '0') {
	on_scanner_error(scanner);
	return;
    }

    /* We've got what we're looking for, skip anything until newline */
    pj_scan_skip_line(scanner);
}