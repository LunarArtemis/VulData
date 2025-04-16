static int
ctnetlink_exp_dump_table(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	struct nf_conntrack_expect *exp, *last;
	struct nfgenmsg *nfmsg = nlmsg_data(cb->nlh);
	u_int8_t l3proto = nfmsg->nfgen_family;

	rcu_read_lock();
	last = (struct nf_conntrack_expect *)cb->args[1];
	for (; cb->args[0] < nf_ct_expect_hsize; cb->args[0]++) {
restart:
		hlist_for_each_entry_rcu(exp, &nf_ct_expect_hash[cb->args[0]],
					 hnode) {
			if (l3proto && exp->tuple.src.l3num != l3proto)
				continue;

			if (!net_eq(nf_ct_net(exp->master), net))
				continue;

			if (cb->args[1]) {
				if (exp != last)
					continue;
				cb->args[1] = 0;
			}
			if (ctnetlink_exp_fill_info(skb,
						    NETLINK_CB(cb->skb).portid,
						    cb->nlh->nlmsg_seq,
						    IPCTNL_MSG_EXP_NEW,
						    exp) < 0) {
				if (!refcount_inc_not_zero(&exp->use))
					continue;
				cb->args[1] = (unsigned long)exp;
				goto out;
			}
		}
		if (cb->args[1]) {
			cb->args[1] = 0;
			goto restart;
		}
	}
out:
	rcu_read_unlock();
	if (last)
		nf_ct_expect_put(last);

	return skb->len;
}