PJ_DEF(void) pj_scan_get_newline( pj_scanner *scanner )
{
    if (!PJ_SCAN_IS_NEWLINE(*scanner->curptr)) {
	pj_scan_syntax_err(scanner);
	return;
    }

    if (*scanner->curptr == '\r') {
	++scanner->curptr;
    }
    if (*scanner->curptr == '\n') {
	++scanner->curptr;
    }

    ++scanner->line;
    scanner->start_line = scanner->curptr;

    /**
     * This probably is a bug, see PROTOS test #2480.
     * This would cause scanner to incorrectly eat two new lines, e.g.
     * when parsing:
     *   
     *	Content-Length: 120\r\n
     *	\r\n
     *	<space><space><space>...
     *
     * When pj_scan_get_newline() is called to parse the first newline
     * in the Content-Length header, it will eat the second newline
     * too because it thinks that it's a header continuation.
     *
     * if (PJ_SCAN_IS_PROBABLY_SPACE(*scanner->curptr) && scanner->skip_ws) {
     *    pj_scan_skip_whitespace(scanner);
     * }
     */
}