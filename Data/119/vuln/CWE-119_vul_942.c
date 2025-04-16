static guint32
parse_wbxml_attribute_list_defined (proto_tree *tree, tvbuff_t *tvb,
				    guint32 offset, guint32 str_tbl, guint8 level, guint8 *codepage_attr,
				    const wbxml_decoding *map)
{
	guint32     tvb_len = tvb_reported_length (tvb);
	guint32     off     = offset;
	guint32     len;
	guint       str_len;
	guint32     ent;
	guint32     idx;
	guint8      peek;
	guint8      attr_save_known   = 0; /* Will contain peek & 0x3F (attr identity) */
	const char *attr_save_literal = NULL; /* Will contain the LITERAL attr identity */

	DebugLog(("parse_wbxml_attr_defined (level = %u, offset = %u)\n",
		  level, offset));
	/* Parse attributes */
	while (off < tvb_len) {
		peek = tvb_get_guint8 (tvb, off);
		DebugLog(("ATTR: (top of while) level = %3u, peek = 0x%02X, "
			  "off = %u, tvb_len = %u\n", level, peek, off, tvb_len));
		if ((peek & 0x3F) < 5) switch (peek) { /* Global tokens
							  in state = ATTR */
		case 0x00: /* SWITCH_PAGE */
			*codepage_attr = tvb_get_guint8 (tvb, off+1);
			proto_tree_add_text (tree, tvb, off, 2,
					     "      |  Attr | A -->%3d "
					     "| SWITCH_PAGE (Attr code page)    |",
					     *codepage_attr);
			off += 2;
			break;
		case 0x01: /* END */
			/* BEWARE
			 *   The Attribute END token means either ">" or "/>"
			 *   and as a consequence both must be treated separately.
			 *   This is done in the TAG state parser.
			 */
			off++;
			DebugLog(("ATTR: level = %u, Return: len = %u\n",
				  level, off - offset));
			return (off - offset);
		case 0x02: /* ENTITY */
			ent = tvb_get_guintvar (tvb, off+1, &len);
			proto_tree_add_text (tree, tvb, off, 1+len,
					     "  %3d |  Attr | A %3d    "
					     "| ENTITY                          "
					     "|     %s'&#%u;'",
					     level, *codepage_attr, Indent (level), ent);
			off += 1+len;
			break;
		case 0x03: /* STR_I */
			len = tvb_strsize (tvb, off+1);
			proto_tree_add_text (tree, tvb, off, 1+len,
					     "  %3d |  Attr | A %3d    "
					     "| STR_I (Inline string)           "
					     "|     %s\'%s\'",
					     level, *codepage_attr, Indent (level),
					     tvb_format_text (tvb, off+1, len-1));
			off += 1+len;
			break;
		case 0x04: /* LITERAL */
			/* ALWAYS means the start of a new attribute,
			 * and may only contain the NAME of the attribute.
			 */
			idx = tvb_get_guintvar (tvb, off+1, &len);
			str_len = tvb_strsize (tvb, str_tbl+idx);
			attr_save_known = 0;
			attr_save_literal = tvb_format_text (tvb,
							     str_tbl+idx, str_len-1);
			proto_tree_add_text (tree, tvb, off, 1+len,
					     "  %3d |  Attr | A %3d    "
					     "| LITERAL (Literal Attribute)     "
					     "|   %s<%s />",
					     level, *codepage_attr, Indent (level),
					     attr_save_literal);
			off += 1+len;
			break;
		case 0x40: /* EXT_I_0 */
		case 0x41: /* EXT_I_1 */
		case 0x42: /* EXT_I_2 */
			/* Extension tokens */
			len = tvb_strsize (tvb, off+1);
			proto_tree_add_text (tree, tvb, off, 1+len,
					     "  %3d |  Attr | A %3d    "
					     "| EXT_I_%1x    (Extension Token)    "
					     "|     %s(%s: \'%s\')",
					     level, *codepage_attr, peek & 0x0f, Indent (level),
					     map_token (map->global, 0, peek),
					     tvb_format_text (tvb, off+1, len-1));
			off += 1+len;
			break;
			/* 0x43 impossible in ATTR state */
			/* 0x44 impossible in ATTR state */
		case 0x80: /* EXT_T_0 */
		case 0x81: /* EXT_T_1 */
		case 0x82: /* EXT_T_2 */
			/* Extension tokens */
			idx = tvb_get_guintvar (tvb, off+1, &len);
			{   char *s;

				if (map->ext_t[peek & 0x03])
					s = (map->ext_t[peek & 0x03])(tvb, idx, str_tbl);
				else
					s = wmem_strdup_printf(wmem_packet_scope(), "EXT_T_%1x (%s)", peek & 0x03,
							    map_token (map->global, 0, peek));

				proto_tree_add_text (tree, tvb, off, 1+len,
						     "  %3d | Tag   | T %3d    "
						     "| EXT_T_%1x    (Extension Token)    "
						     "| %s%s)",
						     level, *codepage_attr, peek & 0x0f, Indent (level),
						     s);
			}
			off += 1+len;
			break;
		case 0x83: /* STR_T */
			idx = tvb_get_guintvar (tvb, off+1, &len);
			str_len = tvb_strsize (tvb, str_tbl+idx);
			proto_tree_add_text (tree, tvb, off, 1+len,
					     "  %3d |  Attr | A %3d    "
					     "| STR_T (Tableref string)         "
					     "|     %s\'%s\'",
					     level, *codepage_attr, Indent (level),
					     tvb_format_text (tvb, str_tbl+idx, str_len-1));
			off += 1+len;
			break;
			/* 0x84 impossible in ATTR state */
		case 0xC0: /* EXT_0 */
		case 0xC1: /* EXT_1 */
		case 0xC2: /* EXT_2 */
			/* Extension tokens */
			proto_tree_add_text (tree, tvb, off, 1,
					     "  %3d |  Attr | A %3d    "
					     "| EXT_%1x      (Extension Token)    "
					     "|     %s(%s)",
					     level, *codepage_attr, peek & 0x0f, Indent (level),
					     map_token (map->global, 0, peek));
			off++;
			break;
		case 0xC3: /* OPAQUE - WBXML 1.1 and newer */
			if (tvb_get_guint8 (tvb, 0)) { /* WBXML 1.x (x > 0) */
				char *str;
				if (attr_save_known) { /* Knwon attribute */
					if (map->opaque_binary_attr) {
						str = map->opaque_binary_attr(tvb, off + 1,
									      attr_save_known, *codepage_attr, &len);
					} else {
						str = default_opaque_binary_attr(tvb, off + 1,
										 attr_save_known, *codepage_attr, &len);
					}
				} else { /* lITERAL attribute */
					if (map->opaque_literal_tag) {
						str = map->opaque_literal_attr(tvb, off + 1,
									       attr_save_literal, *codepage_attr, &len);
					} else {
						str = default_opaque_literal_attr(tvb, off + 1,
										  attr_save_literal, *codepage_attr, &len);
					}
				}
				proto_tree_add_text (tree, tvb, off, 1 + len,
						     "  %3d |  Attr | A %3d    "
						     "| OPAQUE (Opaque data)            "
						     "|       %s%s",
						     level, *codepage_attr, Indent (level), str);
				off += 1 + len;
			} else { /* WBXML 1.0 - RESERVED_2 token (invalid) */
				proto_tree_add_text (tree, tvb, off, 1,
						     "  %3d |  Attr | A %3d    "
						     "| RESERVED_2     (Invalid Token!) "
						     "| WBXML 1.0 parsing stops here.",
						     level, *codepage_attr);
				/* Stop processing as it is impossible to parse now */
				off = tvb_len;
				DebugLog(("ATTR: level = %u, Return: len = %u\n",
					  level, off - offset));
				return (off - offset);
			}
			break;
			/* 0xC4 impossible in ATTR state */
		default:
			proto_tree_add_text (tree, tvb, off, 1,
					     "  %3d |  Attr | A %3d    "
					     "| %-10s     (Invalid Token!) "
					     "| WBXML parsing stops here.",
					     level, *codepage_attr,
					     val_to_str_ext (peek, &vals_wbxml1x_global_tokens_ext, "(unknown 0x%x)"));
			/* Move to end of buffer */
			off = tvb_len;
			break;
		} else { /* Known atribute token */
			if (peek & 0x80) { /* attrValue */
				proto_tree_add_text (tree, tvb, off, 1,
						     "  %3d |  Attr | A %3d    "
						     "|   Known attrValue 0x%02X          "
						     "|       %s%s",
						     level, *codepage_attr, peek & 0x7f, Indent (level),
						     map_token (map->attrValue, *codepage_attr, peek));
				off++;
			} else { /* attrStart */
				attr_save_known = peek & 0x7f;
				proto_tree_add_text (tree, tvb, off, 1,
						     "  %3d |  Attr | A %3d    "
						     "|   Known attrStart 0x%02X          "
						     "|   %s%s",
						     level, *codepage_attr, attr_save_known, Indent (level),
						     map_token (map->attrStart, *codepage_attr, peek));
				off++;
			}
		}
	} /* End WHILE */
	DebugLog(("ATTR: level = %u, Return: len = %u (end of function body)\n",
		  level, off - offset));
	return (off - offset);
}