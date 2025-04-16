static int ctnetlink_dump_mark(struct sk_buff *skb, const struct nf_conn *ct)
{
	if (nla_put_be32(skb, CTA_MARK, htonl(ct->mark)))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -1;
}