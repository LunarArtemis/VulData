static int ctnetlink_stat_exp_cpu(struct net *net, struct sock *ctnl,
				  struct sk_buff *skb,
				  const struct nlmsghdr *nlh,
				  const struct nlattr * const cda[],
				  struct netlink_ext_ack *extack)
{
	if (nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.dump = ctnetlink_exp_stat_cpu_dump,
		};
		return netlink_dump_start(ctnl, skb, nlh, &c);
	}

	return 0;
}