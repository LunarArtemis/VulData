static void php_wddx_push_element(void *user_data, const XML_Char *name, const XML_Char **atts)
{
	st_entry ent;
	wddx_stack *stack = (wddx_stack *)user_data;
	if (!strcmp((char *)name, EL_PACKET)) {
		int i;

		if (atts) for (i=0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_VERSION)) {
				/* nothing for now */
			}
		}
	} else if (!strcmp((char *)name, EL_STRING)) {
		ent.type = ST_STRING;
		SET_STACK_VARNAME;

		ZVAL_STR(&ent.data, ZSTR_EMPTY_ALLOC());
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_BINARY)) {
		ent.type = ST_BINARY;
		SET_STACK_VARNAME;

		ZVAL_STR(&ent.data, ZSTR_EMPTY_ALLOC());
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_CHAR)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_CHAR_CODE) && atts[++i] && atts[i][0]) {
				char tmp_buf[2];

				snprintf(tmp_buf, sizeof(tmp_buf), "%c", (char)strtol((char *)atts[i], NULL, 16));
				php_wddx_process_data(user_data, (XML_Char *) tmp_buf, strlen(tmp_buf));
				break;
			}
		}
	} else if (!strcmp((char *)name, EL_NUMBER)) {
		ent.type = ST_NUMBER;
		SET_STACK_VARNAME;

		ZVAL_LONG(&ent.data, 0);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_BOOLEAN)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_VALUE) && atts[++i] && atts[i][0]) {
				ent.type = ST_BOOLEAN;
				SET_STACK_VARNAME;

				ZVAL_TRUE(&ent.data);
				wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
				php_wddx_process_data(user_data, atts[i], strlen((char *)atts[i]));
				break;
			}
		}
	} else if (!strcmp((char *)name, EL_NULL)) {
		ent.type = ST_NULL;
		SET_STACK_VARNAME;

		ZVAL_NULL(&ent.data);

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_ARRAY)) {
		ent.type = ST_ARRAY;
		SET_STACK_VARNAME;

		array_init(&ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_STRUCT)) {
		ent.type = ST_STRUCT;
		SET_STACK_VARNAME;
		array_init(&ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_VAR)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_NAME) && atts[++i] && atts[i][0]) {
				if (stack->varname) efree(stack->varname);
				stack->varname = estrdup((char *)atts[i]);
				break;
			}
		}
	} else if (!strcmp((char *)name, EL_RECORDSET)) {
		int i;

		ent.type = ST_RECORDSET;
		SET_STACK_VARNAME;
		array_init(&ent.data);

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], "fieldNames") && atts[++i] && atts[i][0]) {
				zval tmp;
				char *key;
				const char *p1, *p2, *endp;

				endp = (char *)atts[i] + strlen((char *)atts[i]);
				p1 = (char *)atts[i];
				while ((p2 = php_memnstr(p1, ",", sizeof(",")-1, endp)) != NULL) {
					key = estrndup(p1, p2 - p1);
					array_init(&tmp);
					add_assoc_zval_ex(&ent.data, key, p2 - p1, &tmp);
					p1 = p2 + sizeof(",")-1;
					efree(key);
				}

				if (p1 <= endp) {
					array_init(&tmp);
					add_assoc_zval_ex(&ent.data, p1, endp - p1, &tmp);
				}

				break;
			}
		}

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_FIELD)) {
		int i;
		st_entry ent;

		ent.type = ST_FIELD;
		ent.varname = NULL;
		ZVAL_UNDEF(&ent.data);

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_NAME) && atts[++i] && atts[i][0]) {
				st_entry *recordset;
				zval *field;

				if (wddx_stack_top(stack, (void**)&recordset) == SUCCESS &&
					recordset->type == ST_RECORDSET &&
					(field = zend_hash_str_find(Z_ARRVAL(recordset->data), (char*)atts[i], strlen((char *)atts[i]))) != NULL) {
					ZVAL_COPY_VALUE(&ent.data, field);
				}

				break;
			}
		}

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_DATETIME)) {
		ent.type = ST_DATETIME;
		SET_STACK_VARNAME;

		ZVAL_LONG(&ent.data, 0);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	}
}