static int ctnetlink_flush_conntrack(struct net *net,
				     const struct nlattr * const cda[],
				     u32 portid, int report, u8 family)
{
	struct ctnetlink_filter *filter = NULL;

	if (ctnetlink_needs_filter(family, cda)) {
		if (cda[CTA_FILTER])
			return -EOPNOTSUPP;

		filter = ctnetlink_alloc_filter(cda, family);
		if (IS_ERR(filter))
			return PTR_ERR(filter);
	}

	nf_ct_iterate_cleanup_net(net, ctnetlink_flush_iterate, filter,
				  portid, report);
	kfree(filter);

	return 0;
}