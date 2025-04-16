static int
dissect_spdu(tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree *tree,
	     gboolean tokens, gboolean connectionless)
{
	gboolean has_user_information = FALSE;
	guint8 type;
	proto_item *ti = NULL;
	proto_tree *ses_tree = NULL;
	int len_len;
	guint16 parameters_len;
	tvbuff_t *next_tvb = NULL;
	guint32 *pres_ctx_id = NULL;
	guint8 enclosure_item_flags = BEGINNING_SPDU|END_SPDU;
	struct SESSION_DATA_STRUCTURE session;

	/*
	 * Get SPDU type.
	 */
	type = tvb_get_guint8(tvb, offset);
	session.spdu_type = type;
	session.abort_type = SESSION_NO_ABORT;
	session.ros_op = 0;
	session.rtse_reassemble = FALSE;

	if(connectionless) {
		col_add_str(pinfo->cinfo, COL_INFO,
			    val_to_str(type, ses_vals, "Unknown SPDU type (0x%02x)"));
		if (tree) {
			ti = proto_tree_add_item(tree, proto_clses, tvb, offset,
				-1, ENC_NA);
			ses_tree = proto_item_add_subtree(ti, ett_ses);
			proto_tree_add_uint(ses_tree, hf_ses_type, tvb,
				offset, 1, type);
		}
		has_user_information = TRUE;
	}
	else if (tokens) {
		col_add_str(pinfo->cinfo, COL_INFO,
			    val_to_str(type, ses_category0_vals, "Unknown SPDU type (0x%02x)"));
		if (tree) {
			ti = proto_tree_add_item(tree, proto_ses, tvb, offset,
			    -1, ENC_NA);
			ses_tree = proto_item_add_subtree(ti, ett_ses);
			proto_tree_add_uint(ses_tree, hf_ses_type_0, tvb,
			    offset, 1, type);
		}
	} else {
		col_add_str(pinfo->cinfo, COL_INFO,
			    val_to_str(type, ses_vals, "Unknown SPDU type (0x%02x)"));
		if (tree) {
			ti = proto_tree_add_item(tree, proto_ses, tvb, offset,
				-1, ENC_NA);
			ses_tree = proto_item_add_subtree(ti, ett_ses);
			proto_tree_add_uint(ses_tree, hf_ses_type, tvb,
				offset, 1, type);
		}

		/*
		 * Might this SPDU have a User Information field?
		 */
		switch (type) {
		case SES_DATA_TRANSFER:
		case SES_EXPEDITED:
		case SES_TYPED_DATA:
			has_user_information = TRUE;
			break;
		case SES_MAJOR_SYNC_POINT:
			pres_ctx_id = (guint32 *)p_get_proto_data(wmem_file_scope(), pinfo, proto_ses, 0);
			if (ses_rtse_reassemble != 0 && !pres_ctx_id) {
				/* First time visited - save pres_ctx_id */
				pres_ctx_id = wmem_new(wmem_file_scope(), guint32);
				*pres_ctx_id = ses_pres_ctx_id;
				p_add_proto_data(wmem_file_scope(), pinfo, proto_ses, 0, pres_ctx_id);
			}
			if (pres_ctx_id) {
				session.pres_ctx_id = *pres_ctx_id;
				session.rtse_reassemble = TRUE;
				has_user_information = TRUE;
			}
			ses_rtse_reassemble = FALSE;
			break;
		}
	}
	offset++;

	/* get length of SPDU parameter field */
	parameters_len = get_item_len(tvb, offset, &len_len);
	if (tree)
		proto_tree_add_uint(ses_tree, hf_ses_length, tvb, offset,
		    len_len, parameters_len);
	offset += len_len;

	/* Dissect parameters. */
	if (!dissect_parameters(tvb, offset, parameters_len, tree, ses_tree,
				pinfo, &enclosure_item_flags, &session))
		has_user_information = FALSE;
	offset += parameters_len;

	proto_item_set_end(ti, tvb, offset);

	/* Dissect user information, if present */
	if (!ses_desegment || enclosure_item_flags == (BEGINNING_SPDU|END_SPDU)) {
		if (has_user_information) {
			/* Not desegment or only one segment */
			if (tvb_reported_length_remaining(tvb, offset) > 0 || type == SES_MAJOR_SYNC_POINT) {
				next_tvb = tvb_new_subset_remaining(tvb, offset);
			}
		}
	} else {
		conversation_t *conversation = NULL;
		fragment_head *frag_msg = NULL;
		gint fragment_len;
		guint32 ses_id = 0;

		/* Use conversation index as segment id */
		conversation  = find_conversation (pinfo->fd->num,
						   &pinfo->src, &pinfo->dst, pinfo->ptype,
						   pinfo->srcport, pinfo->destport, 0);
		if (conversation != NULL) {
			ses_id = conversation->index;
		}
		fragment_len = tvb_reported_length_remaining (tvb, offset);
		ti = proto_tree_add_item (ses_tree, hf_ses_segment_data, tvb, offset,
					  fragment_len, ENC_NA);
		proto_item_append_text (ti, " (%d byte%s)", fragment_len, plurality (fragment_len, "", "s"));
		frag_msg = fragment_add_seq_next (&ses_reassembly_table,
						  tvb, offset,
						  pinfo, ses_id, NULL,
						  fragment_len,
						  (enclosure_item_flags & END_SPDU) ? FALSE : TRUE);
		next_tvb = process_reassembled_data (tvb, offset, pinfo, "Reassembled SES",
						     frag_msg, &ses_frag_items, NULL,
						     (enclosure_item_flags & END_SPDU) ? tree : ses_tree);

		has_user_information = TRUE;
		offset += fragment_len;
	}

	if (has_user_information && next_tvb) {
		if (!pres_handle) {
			call_dissector(data_handle, next_tvb, pinfo, tree);
		} else {
			/* Pass the session pdu to the presentation dissector */
			call_dissector_with_data(pres_handle, next_tvb, pinfo, tree, &session);
		}

		/*
		 * No more SPDUs to dissect.  Set the offset to the
		 * end of the tvbuff.
		 */
		offset = tvb_length(tvb);
		if (session.rtse_reassemble && type == SES_DATA_TRANSFER) {
			ses_pres_ctx_id = session.pres_ctx_id;
			ses_rtse_reassemble = TRUE;
		}
	}
	return offset;
}