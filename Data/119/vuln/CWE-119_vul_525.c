void _xml_characterDataHandler(void *userData, const XML_Char *s, int len)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser) {
		zval retval, args[2];

		if (!Z_ISUNDEF(parser->characterDataHandler)) {
			ZVAL_COPY(&args[0], &parser->index);
			_xml_xmlchar_zval(s, len, parser->target_encoding, &args[1]);
			xml_call_handler(parser, &parser->characterDataHandler, parser->characterDataPtr, 2, args, &retval);
			zval_ptr_dtor(&retval);
		} 

		if (!Z_ISUNDEF(parser->data)) {
			size_t i;
			int doprint = 0;
			zend_string *decoded_value;

			decoded_value = xml_utf8_decode(s, len, parser->target_encoding);
			for (i = 0; i < ZSTR_LEN(decoded_value); i++) {
				switch (ZSTR_VAL(decoded_value)[i]) {
					case ' ':
					case '\t':
					case '\n':
						continue;
					default:
						doprint = 1;
						break;
				}
				if (doprint) {
					break;
				}
			}
			if (doprint || (! parser->skipwhite)) {
				if (parser->lastwasopen) {
					zval *myval;
					
					/* check if the current tag already has a value - if yes append to that! */
					if ((myval = zend_hash_str_find(Z_ARRVAL_P(parser->ctag), "value", sizeof("value") - 1))) {
						int newlen = Z_STRLEN_P(myval) + ZSTR_LEN(decoded_value);
						Z_STR_P(myval) = zend_string_extend(Z_STR_P(myval), newlen, 0);
						strncpy(Z_STRVAL_P(myval) + Z_STRLEN_P(myval) - ZSTR_LEN(decoded_value),
								ZSTR_VAL(decoded_value), ZSTR_LEN(decoded_value) + 1);
						zend_string_release(decoded_value);
					} else {
						add_assoc_str(parser->ctag, "value", decoded_value);
					}
					
				} else {
					zval tag;
					zval *curtag, *mytype, *myval;

					ZEND_HASH_REVERSE_FOREACH_VAL(Z_ARRVAL(parser->data), curtag) {
						if ((mytype = zend_hash_str_find(Z_ARRVAL_P(curtag),"type", sizeof("type") - 1))) {
							if (!strcmp(Z_STRVAL_P(mytype), "cdata")) {
								if ((myval = zend_hash_str_find(Z_ARRVAL_P(curtag), "value", sizeof("value") - 1))) {
									int newlen = Z_STRLEN_P(myval) + ZSTR_LEN(decoded_value);
									Z_STR_P(myval) = zend_string_extend(Z_STR_P(myval), newlen, 0);
									strncpy(Z_STRVAL_P(myval) + Z_STRLEN_P(myval) - ZSTR_LEN(decoded_value),
											ZSTR_VAL(decoded_value), ZSTR_LEN(decoded_value) + 1);
									zend_string_release(decoded_value);
									return;
								}
							}
						}
						break;
					} ZEND_HASH_FOREACH_END();

					if (parser->level <= XML_MAXLEVEL) {
						array_init(&tag);

						_xml_add_to_info(parser,parser->ltags[parser->level-1] + parser->toffset);

						add_assoc_string(&tag, "tag", parser->ltags[parser->level-1] + parser->toffset);
						add_assoc_str(&tag, "value", decoded_value);
						add_assoc_string(&tag, "type", "cdata");
						add_assoc_long(&tag, "level", parser->level);

						zend_hash_next_index_insert(Z_ARRVAL(parser->data), &tag);
					} else if (parser->level == (XML_MAXLEVEL + 1)) {
											php_error_docref(NULL, E_WARNING, "Maximum depth exceeded - Results truncated");
					}
				}
			} else {
				zend_string_release(decoded_value);
			}
		}
	}
}