static int
ctnetlink_dump_table(struct sk_buff *skb, struct netlink_callback *cb)
{
	unsigned int flags = cb->data ? NLM_F_DUMP_FILTERED : 0;
	struct net *net = sock_net(skb->sk);
	struct nf_conn *ct, *last;
	struct nf_conntrack_tuple_hash *h;
	struct hlist_nulls_node *n;
	struct nf_conn *nf_ct_evict[8];
	int res, i;
	spinlock_t *lockp;

	last = (struct nf_conn *)cb->args[1];
	i = 0;

	local_bh_disable();
	for (; cb->args[0] < nf_conntrack_htable_size; cb->args[0]++) {
restart:
		while (i) {
			i--;
			if (nf_ct_should_gc(nf_ct_evict[i]))
				nf_ct_kill(nf_ct_evict[i]);
			nf_ct_put(nf_ct_evict[i]);
		}

		lockp = &nf_conntrack_locks[cb->args[0] % CONNTRACK_LOCKS];
		nf_conntrack_lock(lockp);
		if (cb->args[0] >= nf_conntrack_htable_size) {
			spin_unlock(lockp);
			goto out;
		}
		hlist_nulls_for_each_entry(h, n, &nf_conntrack_hash[cb->args[0]],
					   hnnode) {
			if (NF_CT_DIRECTION(h) != IP_CT_DIR_ORIGINAL)
				continue;
			ct = nf_ct_tuplehash_to_ctrack(h);
			if (nf_ct_is_expired(ct)) {
				if (i < ARRAY_SIZE(nf_ct_evict) &&
				    atomic_inc_not_zero(&ct->ct_general.use))
					nf_ct_evict[i++] = ct;
				continue;
			}

			if (!net_eq(net, nf_ct_net(ct)))
				continue;

			if (cb->args[1]) {
				if (ct != last)
					continue;
				cb->args[1] = 0;
			}
			if (!ctnetlink_filter_match(ct, cb->data))
				continue;

			res =
			ctnetlink_fill_info(skb, NETLINK_CB(cb->skb).portid,
					    cb->nlh->nlmsg_seq,
					    NFNL_MSG_TYPE(cb->nlh->nlmsg_type),
					    ct, true, flags);
			if (res < 0) {
				nf_conntrack_get(&ct->ct_general);
				cb->args[1] = (unsigned long)ct;
				spin_unlock(lockp);
				goto out;
			}
		}
		spin_unlock(lockp);
		if (cb->args[1]) {
			cb->args[1] = 0;
			goto restart;
		}
	}
out:
	local_bh_enable();
	if (last) {
		/* nf ct hash resize happened, now clear the leftover. */
		if ((struct nf_conn *)cb->args[1] == last)
			cb->args[1] = 0;

		nf_ct_put(last);
	}

	while (i) {
		i--;
		if (nf_ct_should_gc(nf_ct_evict[i]))
			nf_ct_kill(nf_ct_evict[i]);
		nf_ct_put(nf_ct_evict[i]);
	}

	return skb->len;
}