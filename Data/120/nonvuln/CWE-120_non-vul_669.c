static int ctnetlink_dump_tuples_ip(struct sk_buff *skb,
				    const struct nf_conntrack_tuple *tuple)
{
	int ret = 0;
	struct nlattr *nest_parms;

	nest_parms = nla_nest_start(skb, CTA_TUPLE_IP);
	if (!nest_parms)
		goto nla_put_failure;

	switch (tuple->src.l3num) {
	case NFPROTO_IPV4:
		ret = ipv4_tuple_to_nlattr(skb, tuple);
		break;
	case NFPROTO_IPV6:
		ret = ipv6_tuple_to_nlattr(skb, tuple);
		break;
	}

	nla_nest_end(skb, nest_parms);

	return ret;

nla_put_failure:
	return -1;
}