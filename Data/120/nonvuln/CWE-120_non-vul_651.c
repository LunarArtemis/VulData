static int ctnetlink_filter_match_tuple(struct nf_conntrack_tuple *filter_tuple,
					struct nf_conntrack_tuple *ct_tuple,
					u_int32_t flags, int family)
{
	switch (family) {
	case NFPROTO_IPV4:
		if ((flags & CTA_FILTER_FLAG(CTA_IP_SRC)) &&
		    filter_tuple->src.u3.ip != ct_tuple->src.u3.ip)
			return  0;

		if ((flags & CTA_FILTER_FLAG(CTA_IP_DST)) &&
		    filter_tuple->dst.u3.ip != ct_tuple->dst.u3.ip)
			return  0;
		break;
	case NFPROTO_IPV6:
		if ((flags & CTA_FILTER_FLAG(CTA_IP_SRC)) &&
		    !ipv6_addr_cmp(&filter_tuple->src.u3.in6,
				   &ct_tuple->src.u3.in6))
			return 0;

		if ((flags & CTA_FILTER_FLAG(CTA_IP_DST)) &&
		    !ipv6_addr_cmp(&filter_tuple->dst.u3.in6,
				   &ct_tuple->dst.u3.in6))
			return 0;
		break;
	}

	if ((flags & CTA_FILTER_FLAG(CTA_PROTO_NUM)) &&
	    filter_tuple->dst.protonum != ct_tuple->dst.protonum)
		return 0;

	switch (ct_tuple->dst.protonum) {
	case IPPROTO_TCP:
	case IPPROTO_UDP:
		if ((flags & CTA_FILTER_FLAG(CTA_PROTO_SRC_PORT)) &&
		    filter_tuple->src.u.tcp.port != ct_tuple->src.u.tcp.port)
			return 0;

		if ((flags & CTA_FILTER_FLAG(CTA_PROTO_DST_PORT)) &&
		    filter_tuple->dst.u.tcp.port != ct_tuple->dst.u.tcp.port)
			return 0;
		break;
	case IPPROTO_ICMP:
		if ((flags & CTA_FILTER_FLAG(CTA_PROTO_ICMP_TYPE)) &&
		    filter_tuple->dst.u.icmp.type != ct_tuple->dst.u.icmp.type)
			return 0;
		if ((flags & CTA_FILTER_FLAG(CTA_PROTO_ICMP_CODE)) &&
		    filter_tuple->dst.u.icmp.code != ct_tuple->dst.u.icmp.code)
			return 0;
		if ((flags & CTA_FILTER_FLAG(CTA_PROTO_ICMP_ID)) &&
		    filter_tuple->src.u.icmp.id != ct_tuple->src.u.icmp.id)
			return 0;
		break;
	case IPPROTO_ICMPV6:
		if ((flags & CTA_FILTER_FLAG(CTA_PROTO_ICMPV6_TYPE)) &&
		    filter_tuple->dst.u.icmp.type != ct_tuple->dst.u.icmp.type)
			return 0;
		if ((flags & CTA_FILTER_FLAG(CTA_PROTO_ICMPV6_CODE)) &&
		    filter_tuple->dst.u.icmp.code != ct_tuple->dst.u.icmp.code)
			return 0;
		if ((flags & CTA_FILTER_FLAG(CTA_PROTO_ICMPV6_ID)) &&
		    filter_tuple->src.u.icmp.id != ct_tuple->src.u.icmp.id)
			return 0;
		break;
	}

	return 1;
}