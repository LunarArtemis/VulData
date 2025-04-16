static int ipv6_nlattr_to_tuple(struct nlattr *tb[],
				struct nf_conntrack_tuple *t,
				u_int32_t flags)
{
	if (flags & CTA_FILTER_FLAG(CTA_IP_SRC)) {
		if (!tb[CTA_IP_V6_SRC])
			return -EINVAL;

		t->src.u3.in6 = nla_get_in6_addr(tb[CTA_IP_V6_SRC]);
	}

	if (flags & CTA_FILTER_FLAG(CTA_IP_DST)) {
		if (!tb[CTA_IP_V6_DST])
			return -EINVAL;

		t->dst.u3.in6 = nla_get_in6_addr(tb[CTA_IP_V6_DST]);
	}

	return 0;
}