static int ctnetlink_exp_dump_mask(struct sk_buff *skb,
				   const struct nf_conntrack_tuple *tuple,
				   const struct nf_conntrack_tuple_mask *mask)
{
	const struct nf_conntrack_l4proto *l4proto;
	struct nf_conntrack_tuple m;
	struct nlattr *nest_parms;
	int ret;

	memset(&m, 0xFF, sizeof(m));
	memcpy(&m.src.u3, &mask->src.u3, sizeof(m.src.u3));
	m.src.u.all = mask->src.u.all;
	m.dst.protonum = tuple->dst.protonum;

	nest_parms = nla_nest_start(skb, CTA_EXPECT_MASK);
	if (!nest_parms)
		goto nla_put_failure;

	rcu_read_lock();
	ret = ctnetlink_dump_tuples_ip(skb, &m);
	if (ret >= 0) {
		l4proto = nf_ct_l4proto_find(tuple->dst.protonum);
		ret = ctnetlink_dump_tuples_proto(skb, &m, l4proto);
	}
	rcu_read_unlock();

	if (unlikely(ret < 0))
		goto nla_put_failure;

	nla_nest_end(skb, nest_parms);

	return 0;

nla_put_failure:
	return -1;
}