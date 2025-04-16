char *build_local_reparse(tm_cell_t *Trans,unsigned int branch,
	unsigned int *len, char *method, int method_len, str *to
	, struct cancel_reason *reason
	)
{
	char	*invite_buf, *invite_buf_end;
	char	*cancel_buf;
	char	*s, *s1, *d;	/* source and destination buffers */
	short	invite_len;
	enum _hdr_types_t	hf_type;
	int	first_via, to_len;
	int cancel_buf_len;
	int reason_len, code_len;
	struct hdr_field *reas1, *reas_last, *hdr;
	int hadded = 0;
	sr_cfgenv_t *cenv = NULL;

	invite_buf = Trans->uac[branch].request.buffer;
	invite_len = Trans->uac[branch].request.buffer_len;

	if (!invite_buf || invite_len<=0) {
		LM_ERR("INVITE is missing\n");
		goto error;
	}
	if ((*invite_buf != 'I') && (*invite_buf != 'i')) {
		LM_ERR("trying to build with local reparse"
					" for a non-INVITE request?\n");
		goto error;
	}

	reason_len = 0;
	reas1 = 0;
	reas_last = 0;
	/* compute reason size (if no reason or disabled => reason_len == 0)*/
	if (reason && reason->cause != CANCEL_REAS_UNKNOWN){
		if (likely(reason->cause > 0 &&
					cfg_get(tm, tm_cfg, local_cancel_reason))){
			/* Reason: SIP;cause=<reason->cause>[;text=<reason->u.text.s>] */
			reason_len = REASON_PREFIX_LEN + USHORT2SBUF_MAX_LEN +
				(reason->u.text.s?
					REASON_TEXT_LEN + 1 + reason->u.text.len + 1 : 0) +
				CRLF_LEN;
		} else if (likely(reason->cause == CANCEL_REAS_PACKED_HDRS &&
					!(Trans->flags & T_NO_E2E_CANCEL_REASON))) {
			reason_len = reason->u.packed_hdrs.len;
		} else if (reason->cause == CANCEL_REAS_RCVD_CANCEL &&
					reason->u.e2e_cancel &&
					!(Trans->flags & T_NO_E2E_CANCEL_REASON)) {
			/* parse the entire cancel, to get all the Reason headers */
			if(parse_headers(reason->u.e2e_cancel, HDR_EOH_F, 0)<0) {
				LM_WARN("failed to parse headers\n");
			}
			for(hdr=get_hdr(reason->u.e2e_cancel, HDR_REASON_T), reas1=hdr;
					hdr; hdr=next_sibling_hdr(hdr)) {
				/* hdr->len includes CRLF */
				reason_len += hdr->len;
				reas_last=hdr;
			}
		} else if (unlikely(reason->cause < CANCEL_REAS_MIN))
			LM_BUG("unhandled reason cause %d\n", reason->cause);
	}

	invite_buf_end = invite_buf + invite_len;
	s = invite_buf;

	/* Allocate memory for the new message.
	The new request will be smaller than the INVITE, so the same size is enough.
	I just extend it with the length of new To HF to be sure.
	Ugly, but we avoid lots of checks and memory allocations this way */
	to_len = to ? to->len : 0;
	cancel_buf_len = invite_len + to_len + reason_len;

	cancel_buf = shm_malloc(sizeof(char)*cancel_buf_len);
	if (!cancel_buf)
	{
		SHM_MEM_ERROR;
		goto error;
	}
	d = cancel_buf;

	/* method name + space */
	append_str(d, method, method_len);
	*d = ' ';
	d++;
	/* skip "INVITE " and copy the rest of the line including CRLF */
	s += 7;
	s1 = s;
	s = eat_line(s, invite_buf_end - s);
	append_str(d, s1, s - s1);

	cenv = sr_cfgenv_get();

	/* check every header field name,
	we must exclude and modify some of the headers */
	first_via = 1;
	while (s < invite_buf_end) {
		s1 = s;
		if ((*s == '\n') || (*s == '\r')) {
			/* end of SIP msg */
			hf_type = HDR_EOH_T;
		} else {
			/* parse HF name */
			s = lw_get_hf_name(s, invite_buf_end,
						&hf_type);
		}

		switch(hf_type) {
			case HDR_CSEQ_T:
				/* find the method name and replace it */
				while ((s < invite_buf_end)
					&& ((*s == ':') || (*s == ' ') || (*s == '\t') ||
						((*s >= '0') && (*s <= '9')))
					) s++;
				append_str(d, s1, s - s1);
				append_str(d, method, method_len);
				append_str(d, CRLF, CRLF_LEN);
				s = lw_next_line(s, invite_buf_end);
				break;

			case HDR_VIA_T:
				s = lw_next_line(s, invite_buf_end);
				if (first_via) {
					/* copy hf */
					append_str(d, s1, s - s1);
					first_via = 0;
				} /* else skip this line, we need olny the first via */
				break;

			case HDR_TO_T:
				if (to_len == 0) {
					/* there is no To tag required, just copy paste
					 * the header */
					s = lw_next_line(s, invite_buf_end);
					append_str(d, s1, s - s1);
				} else {
					/* use the given To HF instead of the original one */
					append_str(d, to->s, to->len);
					/* move the pointer to the next line */
					s = lw_next_line(s, invite_buf_end);
				}
				break;

			case HDR_FROM_T:
			case HDR_CALLID_T:
			case HDR_ROUTE_T:
			case HDR_MAXFORWARDS_T:
				/* copy hf */
				s = lw_next_line(s, invite_buf_end);
				append_str(d, s1, s - s1);
				break;

			case HDR_REQUIRE_T:
			case HDR_PROXYREQUIRE_T:
				/* skip this line */
				s = lw_next_line(s, invite_buf_end);
				break;

			case HDR_CONTENTLENGTH_T:
				/* copy hf name with 0 value */
				append_str(d, s1, s - s1);
				append_str(d, ": 0" CRLF, 3 + CRLF_LEN);
				/* move the pointer to the next line */
				s = lw_next_line(s, invite_buf_end);
				break;

			case HDR_EOH_T:
				/* end of SIP message found */
				/* add reason if needed */
				if (reason_len) {
					/* if reason_len !=0, no need for any reason enabled
					 * checks */
					if (likely(reason->cause > 0)) {
						append_str(d, REASON_PREFIX, REASON_PREFIX_LEN);
						code_len=ushort2sbuf(reason->cause, d,
										cancel_buf_len-(int)(d-cancel_buf));
						if (unlikely(code_len==0))
							LM_BUG("not enough space to write reason code");
						d+=code_len;
						if (reason->u.text.s){
							append_str(d, REASON_TEXT, REASON_TEXT_LEN);
							*d='"'; d++;
							append_str(d, reason->u.text.s,
											reason->u.text.len);
							*d='"'; d++;
						}
						append_str(d, CRLF, CRLF_LEN);
					} else if (likely(reason->cause ==
										CANCEL_REAS_PACKED_HDRS)) {
							append_str(d, reason->u.packed_hdrs.s,
											reason->u.packed_hdrs.len);
					} else if (reason->cause == CANCEL_REAS_RCVD_CANCEL) {
						for(hdr=reas1; hdr; hdr=next_sibling_hdr(hdr)) {
							/* hdr->len includes CRLF */
							append_str(d, hdr->name.s, hdr->len);
							if (likely(hdr==reas_last))
								break;
						}
					}
				}
				/* final (end-of-headers) CRLF */
				append_str(d, CRLF, CRLF_LEN);
				*len = d - cancel_buf;
				/* LOG(L_DBG, "DBG: build_local: %.*s\n", *len, cancel_buf); */
				return cancel_buf;

			default:
				s = lw_next_line(s, invite_buf_end);
				hadded = 0;

				/* uac auth headers */
				if(Trans->uas.request &&
						(Trans->uas.request->msg_flags & FL_UAC_AUTH)) {
					if(s1 + cenv->uac_cseq_auth.len + 2 < invite_buf_end) {
						if(s1[cenv->uac_cseq_auth.len]==':'
								&& strncmp(s1, cenv->uac_cseq_auth.s,
									cenv->uac_cseq_auth.len)==0) {
							hadded = 1;
							append_str(d, s1, s - s1);
						} else if(s1[cenv->uac_cseq_refresh.len]==':'
								&& strncmp(s1, cenv->uac_cseq_refresh.s,
									cenv->uac_cseq_refresh.len)==0) {
							hadded = 1;
							append_str(d, s1, s - s1);
						}
					}
				}

				if(likely(hadded==0)) {
					if (cfg_get(tm, tm_cfg, ac_extra_hdrs).len
							&& (s1 + cfg_get(tm, tm_cfg, ac_extra_hdrs).len < invite_buf_end)
							&& (strncasecmp(s1,
									cfg_get(tm, tm_cfg, ac_extra_hdrs).s,
									cfg_get(tm, tm_cfg, ac_extra_hdrs).len) == 0)) {
						append_str(d, s1, s - s1);
					}
				}
				break;
		}
	}

	/* HDR_EOH_T was not found in the buffer, the message is corrupt */
	LM_ERR("HDR_EOH_T was not found\n");

	shm_free(cancel_buf);
error:
	LM_ERR("cannot build %.*s request\n", method_len, method);
	return NULL;

}