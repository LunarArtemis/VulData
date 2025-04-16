static int
ctnetlink_exp_dump_expect(struct sk_buff *skb,
			  const struct nf_conntrack_expect *exp)
{
	struct nf_conn *master = exp->master;
	long timeout = ((long)exp->timeout.expires - (long)jiffies) / HZ;
	struct nf_conn_help *help;
#if IS_ENABLED(CONFIG_NF_NAT)
	struct nlattr *nest_parms;
	struct nf_conntrack_tuple nat_tuple = {};
#endif
	struct nf_ct_helper_expectfn *expfn;

	if (timeout < 0)
		timeout = 0;

	if (ctnetlink_exp_dump_tuple(skb, &exp->tuple, CTA_EXPECT_TUPLE) < 0)
		goto nla_put_failure;
	if (ctnetlink_exp_dump_mask(skb, &exp->tuple, &exp->mask) < 0)
		goto nla_put_failure;
	if (ctnetlink_exp_dump_tuple(skb,
				 &master->tuplehash[IP_CT_DIR_ORIGINAL].tuple,
				 CTA_EXPECT_MASTER) < 0)
		goto nla_put_failure;

#if IS_ENABLED(CONFIG_NF_NAT)
	if (!nf_inet_addr_cmp(&exp->saved_addr, &any_addr) ||
	    exp->saved_proto.all) {
		nest_parms = nla_nest_start(skb, CTA_EXPECT_NAT);
		if (!nest_parms)
			goto nla_put_failure;

		if (nla_put_be32(skb, CTA_EXPECT_NAT_DIR, htonl(exp->dir)))
			goto nla_put_failure;

		nat_tuple.src.l3num = nf_ct_l3num(master);
		nat_tuple.src.u3 = exp->saved_addr;
		nat_tuple.dst.protonum = nf_ct_protonum(master);
		nat_tuple.src.u = exp->saved_proto;

		if (ctnetlink_exp_dump_tuple(skb, &nat_tuple,
						CTA_EXPECT_NAT_TUPLE) < 0)
	                goto nla_put_failure;
	        nla_nest_end(skb, nest_parms);
	}
#endif
	if (nla_put_be32(skb, CTA_EXPECT_TIMEOUT, htonl(timeout)) ||
	    nla_put_be32(skb, CTA_EXPECT_ID, nf_expect_get_id(exp)) ||
	    nla_put_be32(skb, CTA_EXPECT_FLAGS, htonl(exp->flags)) ||
	    nla_put_be32(skb, CTA_EXPECT_CLASS, htonl(exp->class)))
		goto nla_put_failure;
	help = nfct_help(master);
	if (help) {
		struct nf_conntrack_helper *helper;

		helper = rcu_dereference(help->helper);
		if (helper &&
		    nla_put_string(skb, CTA_EXPECT_HELP_NAME, helper->name))
			goto nla_put_failure;
	}
	expfn = nf_ct_helper_expectfn_find_by_symbol(exp->expectfn);
	if (expfn != NULL &&
	    nla_put_string(skb, CTA_EXPECT_FN, expfn->name))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -1;
}