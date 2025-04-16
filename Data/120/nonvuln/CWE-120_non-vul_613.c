static int ctnetlink_parse_filter(const struct nlattr *attr,
				  struct ctnetlink_filter *filter)
{
	struct nlattr *tb[CTA_FILTER_MAX + 1];
	int ret = 0;

	ret = nla_parse_nested(tb, CTA_FILTER_MAX, attr, cta_filter_nla_policy,
			       NULL);
	if (ret)
		return ret;

	if (tb[CTA_FILTER_ORIG_FLAGS]) {
		filter->orig_flags = nla_get_u32(tb[CTA_FILTER_ORIG_FLAGS]);
		if (filter->orig_flags & ~CTA_FILTER_F_ALL)
			return -EOPNOTSUPP;
	}

	if (tb[CTA_FILTER_REPLY_FLAGS]) {
		filter->reply_flags = nla_get_u32(tb[CTA_FILTER_REPLY_FLAGS]);
		if (filter->reply_flags & ~CTA_FILTER_F_ALL)
			return -EOPNOTSUPP;
	}

	return 0;
}