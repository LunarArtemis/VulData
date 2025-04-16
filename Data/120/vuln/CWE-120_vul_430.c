const char *janus_get_codec_from_pt(const char *sdp, int pt) {
	if(!sdp || pt < 0)
		return NULL;
	if(pt == 0)
		return "pcmu";
	if(pt == 8)
		return "pcma";
	if(pt == 9)
		return "g722";
	/* Look for the mapping */
	char rtpmap[50];
	g_snprintf(rtpmap, 50, "a=rtpmap:%d ", pt);
	const char *line = strstr(sdp, "m=");
	while(line) {
		char *next = strchr(line, '\n');
		if(next) {
			*next = '\0';
			if(strstr(line, rtpmap)) {
				/* Gotcha! */
				char name[100];
				if(sscanf(line, "a=rtpmap:%d %s", &pt, name) == 2) {
					*next = '\n';
					if(strstr(name, "vp8") || strstr(name, "VP8"))
						return "vp8";
					if(strstr(name, "vp9") || strstr(name, "VP9"))
						return "vp9";
					if(strstr(name, "h264") || strstr(name, "H264"))
						return "h264";
					if(strstr(name, "av1") || strstr(name, "AV1"))
						return "av1";
					if(strstr(name, "h265") || strstr(name, "H265"))
						return "h265";
					if(strstr(name, "opus") || strstr(name, "OPUS"))
						return "opus";
					if(strstr(name, "pcmu") || strstr(name, "PCMU"))
						return "pcmu";
					if(strstr(name, "pcma") || strstr(name, "PCMA"))
						return "pcma";
					if(strstr(name, "g722") || strstr(name, "G722"))
						return "g722";
					if(strstr(name, "isac/16") || strstr(name, "ISAC/16"))
						return "isac16";
					if(strstr(name, "isac/32") || strstr(name, "ISAC/32"))
						return "isac32";
					JANUS_LOG(LOG_ERR, "Unsupported codec '%s'\n", name);
					return NULL;
				}
			}
			*next = '\n';
		}
		line = next ? (next+1) : NULL;
	}
	return NULL;
}