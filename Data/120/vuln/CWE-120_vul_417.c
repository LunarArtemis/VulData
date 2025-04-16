static int bgp_capability_software_version(struct peer *peer,
					   struct capability_header *hdr)
{
	struct stream *s = BGP_INPUT(peer);
	char str[BGP_MAX_SOFT_VERSION + 1];
	size_t end = stream_get_getp(s) + hdr->length;
	uint8_t len;

	SET_FLAG(peer->cap, PEER_CAP_SOFT_VERSION_RCV);

	len = stream_getc(s);
	if (stream_get_getp(s) + len > end) {
		flog_warn(
			EC_BGP_CAPABILITY_INVALID_DATA,
			"%s: Received malformed Software Version capability from peer %s",
			__func__, peer->host);
		return -1;
	}

	if (len) {
		stream_get(str, s, len);
		str[len] = '\0';

		XFREE(MTYPE_BGP_SOFT_VERSION, peer->soft_version);

		peer->soft_version = XSTRDUP(MTYPE_BGP_SOFT_VERSION, str);

		if (bgp_debug_neighbor_events(peer))
			zlog_debug("%s sent Software Version: %s", peer->host,
				   peer->soft_version);
	}

	return 0;
}