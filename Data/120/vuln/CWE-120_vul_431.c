const char *janus_rtp_header_extension_get_from_id(const char *sdp, int id) {
	if(!sdp || id < 0)
		return NULL;
	/* Look for the mapping */
	char extmap[100];
	g_snprintf(extmap, 100, "a=extmap:%d ", id);
	const char *line = strstr(sdp, "m=");
	while(line) {
		char *next = strchr(line, '\n');
		if(next) {
			*next = '\0';
			if(strstr(line, extmap)) {
				/* Gotcha! */
				char extension[100];
				if(sscanf(line, "a=extmap:%d %s", &id, extension) == 2) {
					*next = '\n';
					if(strstr(extension, JANUS_RTP_EXTMAP_AUDIO_LEVEL))
						return JANUS_RTP_EXTMAP_AUDIO_LEVEL;
					if(strstr(extension, JANUS_RTP_EXTMAP_VIDEO_ORIENTATION))
						return JANUS_RTP_EXTMAP_VIDEO_ORIENTATION;
					if(strstr(extension, JANUS_RTP_EXTMAP_PLAYOUT_DELAY))
						return JANUS_RTP_EXTMAP_PLAYOUT_DELAY;
					if(strstr(extension, JANUS_RTP_EXTMAP_TOFFSET))
						return JANUS_RTP_EXTMAP_TOFFSET;
					if(strstr(extension, JANUS_RTP_EXTMAP_ABS_SEND_TIME))
						return JANUS_RTP_EXTMAP_ABS_SEND_TIME;
					if(strstr(extension, JANUS_RTP_EXTMAP_TRANSPORT_WIDE_CC))
						return JANUS_RTP_EXTMAP_TRANSPORT_WIDE_CC;
					if(strstr(extension, JANUS_RTP_EXTMAP_MID))
						return JANUS_RTP_EXTMAP_MID;
					if(strstr(extension, JANUS_RTP_EXTMAP_RID))
						return JANUS_RTP_EXTMAP_RID;
					if(strstr(extension, JANUS_RTP_EXTMAP_REPAIRED_RID))
						return JANUS_RTP_EXTMAP_REPAIRED_RID;
					JANUS_LOG(LOG_ERR, "Unsupported extension '%s'\n", extension);
					return NULL;
				}
			}
			*next = '\n';
		}
		line = next ? (next+1) : NULL;
	}
	return NULL;
}