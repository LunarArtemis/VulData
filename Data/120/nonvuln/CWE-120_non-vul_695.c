static int ctnetlink_get_expect(struct net *net, struct sock *ctnl,
				struct sk_buff *skb, const struct nlmsghdr *nlh,
				const struct nlattr * const cda[],
				struct netlink_ext_ack *extack)
{
	struct nf_conntrack_tuple tuple;
	struct nf_conntrack_expect *exp;
	struct sk_buff *skb2;
	struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	u_int8_t u3 = nfmsg->nfgen_family;
	struct nf_conntrack_zone zone;
	int err;

	if (nlh->nlmsg_flags & NLM_F_DUMP) {
		if (cda[CTA_EXPECT_MASTER])
			return ctnetlink_dump_exp_ct(net, ctnl, skb, nlh, cda,
						     extack);
		else {
			struct netlink_dump_control c = {
				.dump = ctnetlink_exp_dump_table,
				.done = ctnetlink_exp_done,
			};
			return netlink_dump_start(ctnl, skb, nlh, &c);
		}
	}

	err = ctnetlink_parse_zone(cda[CTA_EXPECT_ZONE], &zone);
	if (err < 0)
		return err;

	if (cda[CTA_EXPECT_TUPLE])
		err = ctnetlink_parse_tuple(cda, &tuple, CTA_EXPECT_TUPLE,
					    u3, NULL);
	else if (cda[CTA_EXPECT_MASTER])
		err = ctnetlink_parse_tuple(cda, &tuple, CTA_EXPECT_MASTER,
					    u3, NULL);
	else
		return -EINVAL;

	if (err < 0)
		return err;

	exp = nf_ct_expect_find_get(net, &zone, &tuple);
	if (!exp)
		return -ENOENT;

	if (cda[CTA_EXPECT_ID]) {
		__be32 id = nla_get_be32(cda[CTA_EXPECT_ID]);

		if (id != nf_expect_get_id(exp)) {
			nf_ct_expect_put(exp);
			return -ENOENT;
		}
	}

	err = -ENOMEM;
	skb2 = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (skb2 == NULL) {
		nf_ct_expect_put(exp);
		goto out;
	}

	rcu_read_lock();
	err = ctnetlink_exp_fill_info(skb2, NETLINK_CB(skb).portid,
				      nlh->nlmsg_seq, IPCTNL_MSG_EXP_NEW, exp);
	rcu_read_unlock();
	nf_ct_expect_put(exp);
	if (err <= 0)
		goto free;

	err = netlink_unicast(ctnl, skb2, NETLINK_CB(skb).portid, MSG_DONTWAIT);
	if (err < 0)
		goto out;

	return 0;

free:
	kfree_skb(skb2);
out:
	/* this avoids a loop in nfnetlink. */
	return err == -EAGAIN ? -ENOBUFS : err;
}