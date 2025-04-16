static int ctnetlink_exp_dump_tuple(struct sk_buff *skb,
				    const struct nf_conntrack_tuple *tuple,
				    u32 type)
{
	struct nlattr *nest_parms;

	nest_parms = nla_nest_start(skb, type);
	if (!nest_parms)
		goto nla_put_failure;
	if (ctnetlink_dump_tuples(skb, tuple) < 0)
		goto nla_put_failure;
	nla_nest_end(skb, nest_parms);

	return 0;

nla_put_failure:
	return -1;
}