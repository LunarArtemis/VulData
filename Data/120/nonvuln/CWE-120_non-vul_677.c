static int ipv4_nlattr_to_tuple(struct nlattr *tb[],
				struct nf_conntrack_tuple *t,
				u_int32_t flags)
{
	if (flags & CTA_FILTER_FLAG(CTA_IP_SRC)) {
		if (!tb[CTA_IP_V4_SRC])
			return -EINVAL;

		t->src.u3.ip = nla_get_in_addr(tb[CTA_IP_V4_SRC]);
	}

	if (flags & CTA_FILTER_FLAG(CTA_IP_DST)) {
		if (!tb[CTA_IP_V4_DST])
			return -EINVAL;

		t->dst.u3.ip = nla_get_in_addr(tb[CTA_IP_V4_DST]);
	}

	return 0;
}