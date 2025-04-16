static int
ctnetlink_dump_list(struct sk_buff *skb, struct netlink_callback *cb, bool dying)
{
	struct nf_conn *ct, *last;
	struct nf_conntrack_tuple_hash *h;
	struct hlist_nulls_node *n;
	struct nfgenmsg *nfmsg = nlmsg_data(cb->nlh);
	u_int8_t l3proto = nfmsg->nfgen_family;
	int res;
	int cpu;
	struct hlist_nulls_head *list;
	struct net *net = sock_net(skb->sk);

	if (cb->args[2])
		return 0;

	last = (struct nf_conn *)cb->args[1];

	for (cpu = cb->args[0]; cpu < nr_cpu_ids; cpu++) {
		struct ct_pcpu *pcpu;

		if (!cpu_possible(cpu))
			continue;

		pcpu = per_cpu_ptr(net->ct.pcpu_lists, cpu);
		spin_lock_bh(&pcpu->lock);
		list = dying ? &pcpu->dying : &pcpu->unconfirmed;
restart:
		hlist_nulls_for_each_entry(h, n, list, hnnode) {
			ct = nf_ct_tuplehash_to_ctrack(h);
			if (l3proto && nf_ct_l3num(ct) != l3proto)
				continue;
			if (cb->args[1]) {
				if (ct != last)
					continue;
				cb->args[1] = 0;
			}

			/* We can't dump extension info for the unconfirmed
			 * list because unconfirmed conntracks can have
			 * ct->ext reallocated (and thus freed).
			 *
			 * In the dying list case ct->ext can't be free'd
			 * until after we drop pcpu->lock.
			 */
			res = ctnetlink_fill_info(skb, NETLINK_CB(cb->skb).portid,
						  cb->nlh->nlmsg_seq,
						  NFNL_MSG_TYPE(cb->nlh->nlmsg_type),
						  ct, dying ? true : false, 0);
			if (res < 0) {
				if (!atomic_inc_not_zero(&ct->ct_general.use))
					continue;
				cb->args[0] = cpu;
				cb->args[1] = (unsigned long)ct;
				spin_unlock_bh(&pcpu->lock);
				goto out;
			}
		}
		if (cb->args[1]) {
			cb->args[1] = 0;
			goto restart;
		}
		spin_unlock_bh(&pcpu->lock);
	}
	cb->args[2] = 1;
out:
	if (last)
		nf_ct_put(last);

	return skb->len;
}