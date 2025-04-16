static int janus_streaming_rtsp_parse_sdp(const char *buffer, const char *name, const char *media, char *base, int *pt,
		char *transport, char *host, char *rtpmap, char *fmtp, char *control, const janus_network_address *iface, multiple_fds *fds) {
	/* Start by checking if there's any Content-Base header we should be aware of */
	const char *cb = strstr(buffer, "Content-Base:");
	if(cb == NULL)
		cb = strstr(buffer, "content-base:");
	if(cb != NULL) {
		cb = strstr(cb, "rtsp://");
		const char *crlf = (cb ? strstr(cb, "\r\n") : NULL);
		if(crlf != NULL && base != NULL) {
			gulong size = (crlf-cb)+1;
			if(size > 256)
				size = 256;
			g_snprintf(base, size, "%s", cb);
			if(base[size-2] == '/')
				base[size-2] = '\0';
		}
	}
	/* Parse the SDP now */
	char pattern[256];
	g_snprintf(pattern, sizeof(pattern), "m=%s", media);
	char *m = strstr(buffer, pattern);
	if(m == NULL) {
		JANUS_LOG(LOG_VERB, "[%s] no media %s...\n", name, media);
		return -1;
	}
	sscanf(m, "m=%*s %*d %*s %d", pt);
	char *s = strstr(m, "a=control:");
	if(s == NULL) {
		JANUS_LOG(LOG_ERR, "[%s] no control for %s...\n", name, media);
		return -1;
	}
	sscanf(s, "a=control:%2047s", control);
	char *r = strstr(m, "a=rtpmap:");
	if(r != NULL) {
		if (sscanf(r, "a=rtpmap:%*d%*[ ]%2047[^\r\n]s", rtpmap) != 1) {
			JANUS_LOG(LOG_ERR, "[%s] cannot parse %s rtpmap...\n", name, media);
			return -1;
		}
	}
	char *f = strstr(m, "a=fmtp:");
	if(f != NULL) {
		if (sscanf(f, "a=fmtp:%*d%*[ ]%2047[^\r\n]s", fmtp) != 1) {
			JANUS_LOG(LOG_ERR, "[%s] cannot parse %s fmtp...\n", name, media);
			return -1;
		}
	}
	char *c = strstr(m, "c=IN IP4");
	if(c == NULL) {
		/* No m-line c= attribute? try in the whole SDP */
		c = strstr(buffer, "c=IN IP4");
	}
	char ip[256];
	in_addr_t mcast = INADDR_ANY;
	if(c != NULL) {
		if(sscanf(c, "c=IN IP4 %[^/]", ip) != 0) {
			memcpy(host, ip, sizeof(ip));
			c = strstr(host, "\r\n");
			if(c)
				*c = '\0';
			mcast = inet_addr(ip);
		}
	}
	/* Bind two adjacent ports for RTP and RTCP */
	int ports[2];
	if(janus_streaming_allocate_port_pair(name, media, mcast, iface, fds, ports)) {
		JANUS_LOG(LOG_ERR, "[%s] Bind failed for %s...\n", name, media);
		return -1;
	}

	if(IN_MULTICAST(ntohl(mcast))) {
		g_snprintf(transport, 1024, "RTP/AVP/UDP;multicast;client_port=%d-%d", ports[0], ports[1]);
	} else {
		g_snprintf(transport, 1024, "RTP/AVP/UDP;unicast;client_port=%d-%d", ports[0], ports[1]);
	}

	return 0;
}