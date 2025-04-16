static int ctnetlink_parse_tuple_ip(struct nlattr *attr,
				    struct nf_conntrack_tuple *tuple,
				    u_int32_t flags)
{
	struct nlattr *tb[CTA_IP_MAX+1];
	int ret = 0;

	ret = nla_parse_nested_deprecated(tb, CTA_IP_MAX, attr, NULL, NULL);
	if (ret < 0)
		return ret;

	ret = nla_validate_nested_deprecated(attr, CTA_IP_MAX,
					     cta_ip_nla_policy, NULL);
	if (ret)
		return ret;

	switch (tuple->src.l3num) {
	case NFPROTO_IPV4:
		ret = ipv4_nlattr_to_tuple(tb, tuple, flags);
		break;
	case NFPROTO_IPV6:
		ret = ipv6_nlattr_to_tuple(tb, tuple, flags);
		break;
	}

	return ret;
}