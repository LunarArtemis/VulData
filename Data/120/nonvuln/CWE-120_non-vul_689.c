static int ipv6_tuple_to_nlattr(struct sk_buff *skb,
				const struct nf_conntrack_tuple *tuple)
{
	if (nla_put_in6_addr(skb, CTA_IP_V6_SRC, &tuple->src.u3.in6) ||
	    nla_put_in6_addr(skb, CTA_IP_V6_DST, &tuple->dst.u3.in6))
		return -EMSGSIZE;
	return 0;
}