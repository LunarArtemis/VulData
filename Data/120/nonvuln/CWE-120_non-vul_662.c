static struct ctnetlink_filter *
ctnetlink_alloc_filter(const struct nlattr * const cda[], u8 family)
{
	struct ctnetlink_filter *filter;
	int err;

#ifndef CONFIG_NF_CONNTRACK_MARK
	if (cda[CTA_MARK] || cda[CTA_MARK_MASK])
		return ERR_PTR(-EOPNOTSUPP);
#endif

	filter = kzalloc(sizeof(*filter), GFP_KERNEL);
	if (filter == NULL)
		return ERR_PTR(-ENOMEM);

	filter->family = family;

#ifdef CONFIG_NF_CONNTRACK_MARK
	if (cda[CTA_MARK]) {
		filter->mark.val = ntohl(nla_get_be32(cda[CTA_MARK]));
		filter->cta_flags |= CTA_FILTER_FLAG(CTA_MARK);

		if (cda[CTA_MARK_MASK]) {
			filter->mark.mask = ntohl(nla_get_be32(cda[CTA_MARK_MASK]));
			filter->cta_flags |= CTA_FILTER_FLAG(CTA_MARK_MASK);
		} else {
			filter->mark.mask = 0xffffffff;
		}
	} else if (cda[CTA_MARK_MASK]) {
		err = -EINVAL;
		goto err_filter;
	}
#endif
	if (!cda[CTA_FILTER])
		return filter;

	err = ctnetlink_parse_zone(cda[CTA_ZONE], &filter->zone);
	if (err < 0)
		goto err_filter;

	err = ctnetlink_parse_filter(cda[CTA_FILTER], filter);
	if (err < 0)
		goto err_filter;

	if (filter->orig_flags) {
		if (!cda[CTA_TUPLE_ORIG]) {
			err = -EINVAL;
			goto err_filter;
		}

		err = ctnetlink_parse_tuple_filter(cda, &filter->orig,
						   CTA_TUPLE_ORIG,
						   filter->family,
						   &filter->zone,
						   filter->orig_flags);
		if (err < 0)
			goto err_filter;
	}

	if (filter->reply_flags) {
		if (!cda[CTA_TUPLE_REPLY]) {
			err = -EINVAL;
			goto err_filter;
		}

		err = ctnetlink_parse_tuple_filter(cda, &filter->reply,
						   CTA_TUPLE_REPLY,
						   filter->family,
						   &filter->zone,
						   filter->orig_flags);
		if (err < 0) {
			err = -EINVAL;
			goto err_filter;
		}
	}

	return filter;

err_filter:
	kfree(filter);

	return ERR_PTR(err);
}