static int update_header(void *obj)
{
	struct header_data *data = obj;
	pjsip_hdr *hdr = NULL;
	RAII_VAR(struct ast_datastore *, datastore,
			 ast_sip_session_get_datastore(data->channel->session, data->header_datastore->type),
			 ao2_cleanup);

	if (!datastore || !datastore->data) {
		ast_log(AST_LOG_ERROR, "No headers had been previously added to this session.\n");
		return -1;
	}

	hdr = find_header((struct hdr_list *) datastore->data, data->header_name,
					  data->header_number);

	if (!hdr) {
		ast_log(AST_LOG_ERROR, "There was no header named %s.\n", data->header_name);
		return -1;
	}

	pj_strcpy2(&((pjsip_generic_string_hdr *) hdr)->hvalue, data->header_value);

	return 0;
}