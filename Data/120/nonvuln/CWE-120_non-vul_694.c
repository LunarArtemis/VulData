static int ctnetlink_del_expect(struct net *net, struct sock *ctnl,
				struct sk_buff *skb, const struct nlmsghdr *nlh,
				const struct nlattr * const cda[],
				struct netlink_ext_ack *extack)
{
	struct nf_conntrack_expect *exp;
	struct nf_conntrack_tuple tuple;
	struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	u_int8_t u3 = nfmsg->nfgen_family;
	struct nf_conntrack_zone zone;
	int err;

	if (cda[CTA_EXPECT_TUPLE]) {
		/* delete a single expect by tuple */
		err = ctnetlink_parse_zone(cda[CTA_EXPECT_ZONE], &zone);
		if (err < 0)
			return err;

		err = ctnetlink_parse_tuple(cda, &tuple, CTA_EXPECT_TUPLE,
					    u3, NULL);
		if (err < 0)
			return err;

		/* bump usage count to 2 */
		exp = nf_ct_expect_find_get(net, &zone, &tuple);
		if (!exp)
			return -ENOENT;

		if (cda[CTA_EXPECT_ID]) {
			__be32 id = nla_get_be32(cda[CTA_EXPECT_ID]);
			if (ntohl(id) != (u32)(unsigned long)exp) {
				nf_ct_expect_put(exp);
				return -ENOENT;
			}
		}

		/* after list removal, usage count == 1 */
		spin_lock_bh(&nf_conntrack_expect_lock);
		if (del_timer(&exp->timeout)) {
			nf_ct_unlink_expect_report(exp, NETLINK_CB(skb).portid,
						   nlmsg_report(nlh));
			nf_ct_expect_put(exp);
		}
		spin_unlock_bh(&nf_conntrack_expect_lock);
		/* have to put what we 'get' above.
		 * after this line usage count == 0 */
		nf_ct_expect_put(exp);
	} else if (cda[CTA_EXPECT_HELP_NAME]) {
		char *name = nla_data(cda[CTA_EXPECT_HELP_NAME]);

		nf_ct_expect_iterate_net(net, expect_iter_name, name,
					 NETLINK_CB(skb).portid,
					 nlmsg_report(nlh));
	} else {
		/* This basically means we have to flush everything*/
		nf_ct_expect_iterate_net(net, expect_iter_all, NULL,
					 NETLINK_CB(skb).portid,
					 nlmsg_report(nlh));
	}

	return 0;
}