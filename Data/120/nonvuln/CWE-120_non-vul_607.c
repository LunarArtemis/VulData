static int ctnetlink_dump_exp_ct(struct net *net, struct sock *ctnl,
				 struct sk_buff *skb,
				 const struct nlmsghdr *nlh,
				 const struct nlattr * const cda[],
				 struct netlink_ext_ack *extack)
{
	int err;
	struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	u_int8_t u3 = nfmsg->nfgen_family;
	struct nf_conntrack_tuple tuple;
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;
	struct nf_conntrack_zone zone;
	struct netlink_dump_control c = {
		.dump = ctnetlink_exp_ct_dump_table,
		.done = ctnetlink_exp_done,
	};

	err = ctnetlink_parse_tuple(cda, &tuple, CTA_EXPECT_MASTER,
				    u3, NULL);
	if (err < 0)
		return err;

	err = ctnetlink_parse_zone(cda[CTA_EXPECT_ZONE], &zone);
	if (err < 0)
		return err;

	h = nf_conntrack_find_get(net, &zone, &tuple);
	if (!h)
		return -ENOENT;

	ct = nf_ct_tuplehash_to_ctrack(h);
	/* No expectation linked to this connection tracking. */
	if (!nfct_help(ct)) {
		nf_ct_put(ct);
		return 0;
	}

	c.data = ct;

	err = netlink_dump_start(ctnl, skb, nlh, &c);
	nf_ct_put(ct);

	return err;
}