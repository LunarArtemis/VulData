static int ctnetlink_dump_protoinfo(struct sk_buff *skb, struct nf_conn *ct)
{
	const struct nf_conntrack_l4proto *l4proto;
	struct nlattr *nest_proto;
	int ret;

	l4proto = nf_ct_l4proto_find(nf_ct_protonum(ct));
	if (!l4proto->to_nlattr)
		return 0;

	nest_proto = nla_nest_start(skb, CTA_PROTOINFO);
	if (!nest_proto)
		goto nla_put_failure;

	ret = l4proto->to_nlattr(skb, nest_proto, ct);

	nla_nest_end(skb, nest_proto);

	return ret;

nla_put_failure:
	return -1;
}