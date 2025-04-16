static int ctnetlink_dump_tuples(struct sk_buff *skb,
				 const struct nf_conntrack_tuple *tuple)
{
	const struct nf_conntrack_l4proto *l4proto;
	int ret;

	rcu_read_lock();
	ret = ctnetlink_dump_tuples_ip(skb, tuple);

	if (ret >= 0) {
		l4proto = nf_ct_l4proto_find(tuple->dst.protonum);
		ret = ctnetlink_dump_tuples_proto(skb, tuple, l4proto);
	}
	rcu_read_unlock();
	return ret;
}