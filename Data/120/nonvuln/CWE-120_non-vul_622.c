static int ipv4_tuple_to_nlattr(struct sk_buff *skb,
				const struct nf_conntrack_tuple *tuple)
{
	if (nla_put_in_addr(skb, CTA_IP_V4_SRC, tuple->src.u3.ip) ||
	    nla_put_in_addr(skb, CTA_IP_V4_DST, tuple->dst.u3.ip))
		return -EMSGSIZE;
	return 0;
}