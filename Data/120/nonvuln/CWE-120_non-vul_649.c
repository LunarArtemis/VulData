static int
ctnetlink_ct_stat_cpu_dump(struct sk_buff *skb, struct netlink_callback *cb)
{
	int cpu;
	struct net *net = sock_net(skb->sk);

	if (cb->args[0] == nr_cpu_ids)
		return 0;

	for (cpu = cb->args[0]; cpu < nr_cpu_ids; cpu++) {
		const struct ip_conntrack_stat *st;

		if (!cpu_possible(cpu))
			continue;

		st = per_cpu_ptr(net->ct.stat, cpu);
		if (ctnetlink_ct_stat_cpu_fill_info(skb,
						    NETLINK_CB(cb->skb).portid,
						    cb->nlh->nlmsg_seq,
						    cpu, st) < 0)
				break;
	}
	cb->args[0] = cpu;

	return skb->len;
}