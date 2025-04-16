char *janus_sip_sdp_manipulate(janus_sip_session *session, janus_sdp *sdp, gboolean answer) {
	if(!session || !session->stack || !sdp)
		return NULL;
	GHashTable *codecs = NULL;
	GList *pts_to_remove = NULL;
	/* Start replacing stuff */
	JANUS_LOG(LOG_VERB, "Setting protocol to %s\n", session->media.require_srtp ? "RTP/SAVP" : "RTP/AVP");
	if(sdp->c_addr) {
		g_free(sdp->c_addr);
		sdp->c_addr = g_strdup(sdp_ip ? sdp_ip : (local_media_ip ? local_media_ip : local_ip));
	}
	GList *temp = sdp->m_lines;
	while(temp) {
		janus_sdp_mline *m = (janus_sdp_mline *)temp->data;
		g_free(m->proto);
		m->proto = g_strdup(session->media.require_srtp ? "RTP/SAVP" : "RTP/AVP");
		if(m->type == JANUS_SDP_AUDIO) {
			m->port = session->media.local_audio_rtp_port;
			if(session->media.has_srtp_local_audio) {
				if(!session->media.audio_srtp_local_profile || !session->media.audio_srtp_local_crypto) {
					janus_sip_srtp_set_local(session, FALSE, &session->media.audio_srtp_local_profile, &session->media.audio_srtp_local_crypto);
				}
				janus_sdp_attribute *a = janus_sdp_attribute_create("crypto", "1 %s inline:%s", session->media.audio_srtp_local_profile, session->media.audio_srtp_local_crypto);
				m->attributes = g_list_append(m->attributes, a);
			}
		} else if(m->type == JANUS_SDP_VIDEO) {
			m->port = session->media.local_video_rtp_port;
			if(session->media.has_srtp_local_video) {
				if(!session->media.video_srtp_local_profile || !session->media.video_srtp_local_crypto) {
					janus_sip_srtp_set_local(session, TRUE, &session->media.video_srtp_local_profile, &session->media.video_srtp_local_crypto);
				}
				janus_sdp_attribute *a = janus_sdp_attribute_create("crypto", "1 %s inline:%s", session->media.video_srtp_local_profile, session->media.video_srtp_local_crypto);
				m->attributes = g_list_append(m->attributes, a);
			}
		}
		g_free(m->c_addr);
		m->c_addr = g_strdup(sdp_ip ? sdp_ip : (local_media_ip ? local_media_ip : local_ip));
		if(answer && (m->type == JANUS_SDP_AUDIO || m->type == JANUS_SDP_VIDEO)) {
			/* Check which codec was negotiated eventually */
			int pt = -1;
			if(m->ptypes)
				pt = GPOINTER_TO_INT(m->ptypes->data);
			if(pt > -1) {
				if(m->type == JANUS_SDP_AUDIO) {
					session->media.audio_pt = pt;
				} else {
					session->media.video_pt = pt;
				}
			}
		}
		/* If this is an answer, get rid of multiple versions of the same
		 * codec as well (e.g., video profiles), as that confuses the hell
		 * out of SOATAG_RTP_SELECT(SOA_RTP_SELECT_COMMON) in nua_respond() */
		if(answer) {
			if(codecs == NULL)
				codecs = g_hash_table_new_full(g_str_hash, g_str_equal, (GDestroyNotify)g_free, NULL);
			/* Check all rtpmap attributes */
			int pt = -1;
			char codec[50];
			GList *ma = m->attributes;
			while(ma) {
				janus_sdp_attribute *a = (janus_sdp_attribute *)ma->data;
				if(a->name != NULL && a->value != NULL && !strcasecmp(a->name, "rtpmap")) {
					if(sscanf(a->value, "%3d %s", &pt, codec) == 2) {
						if(g_hash_table_lookup(codecs, codec) != NULL) {
							/* We already have a version of this codec, remove the payload type */
							pts_to_remove = g_list_append(pts_to_remove, GINT_TO_POINTER(pt));
							JANUS_LOG(LOG_HUGE, "Removing %d (%s)\n", pt, codec);
						} else {
							/* Keep track of this codec */
							g_hash_table_insert(codecs, g_strdup(codec), GINT_TO_POINTER(pt));
						}
					}
				}
				ma = ma->next;
			}
		}
		temp = temp->next;
	}
	/* If we need to remove some payload types from the SDP, do it now */
	if(pts_to_remove != NULL) {
		GList *temp = pts_to_remove;
		while(temp) {
			int pt = GPOINTER_TO_INT(temp->data);
			janus_sdp_remove_payload_type(sdp, pt);
			temp = temp->next;
		}
		g_list_free(pts_to_remove);
	}
	/* Generate a SDP string out of our changes */
	return janus_sdp_write(sdp);
}