static int ctnetlink_start(struct netlink_callback *cb)
{
	const struct nlattr * const *cda = cb->data;
	struct ctnetlink_filter *filter = NULL;
	struct nfgenmsg *nfmsg = nlmsg_data(cb->nlh);
	u8 family = nfmsg->nfgen_family;

	if (ctnetlink_needs_filter(family, cda)) {
		filter = ctnetlink_alloc_filter(cda, family);
		if (IS_ERR(filter))
			return PTR_ERR(filter);
	}

	cb->data = filter;
	return 0;
}