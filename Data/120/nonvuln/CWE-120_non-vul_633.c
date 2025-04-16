static int
ctnetlink_expect_event(unsigned int events, struct nf_exp_event *item)
{
	struct nf_conntrack_expect *exp = item->exp;
	struct net *net = nf_ct_exp_net(exp);
	struct nlmsghdr *nlh;
	struct nfgenmsg *nfmsg;
	struct sk_buff *skb;
	unsigned int type, group;
	int flags = 0;

	if (events & (1 << IPEXP_DESTROY)) {
		type = IPCTNL_MSG_EXP_DELETE;
		group = NFNLGRP_CONNTRACK_EXP_DESTROY;
	} else if (events & (1 << IPEXP_NEW)) {
		type = IPCTNL_MSG_EXP_NEW;
		flags = NLM_F_CREATE|NLM_F_EXCL;
		group = NFNLGRP_CONNTRACK_EXP_NEW;
	} else
		return 0;

	if (!item->report && !nfnetlink_has_listeners(net, group))
		return 0;

	skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_ATOMIC);
	if (skb == NULL)
		goto errout;

	type = nfnl_msg_type(NFNL_SUBSYS_CTNETLINK_EXP, type);
	nlh = nlmsg_put(skb, item->portid, 0, type, sizeof(*nfmsg), flags);
	if (nlh == NULL)
		goto nlmsg_failure;

	nfmsg = nlmsg_data(nlh);
	nfmsg->nfgen_family = exp->tuple.src.l3num;
	nfmsg->version	    = NFNETLINK_V0;
	nfmsg->res_id	    = 0;

	if (ctnetlink_exp_dump_expect(skb, exp) < 0)
		goto nla_put_failure;

	nlmsg_end(skb, nlh);
	nfnetlink_send(skb, net, item->portid, group, item->report, GFP_ATOMIC);
	return 0;

nla_put_failure:
	nlmsg_cancel(skb, nlh);
nlmsg_failure:
	kfree_skb(skb);
errout:
	nfnetlink_set_err(net, 0, 0, -ENOBUFS);
	return 0;
}