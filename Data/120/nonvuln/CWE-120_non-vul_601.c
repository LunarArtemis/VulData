static int ctnetlink_dump_status(struct sk_buff *skb, const struct nf_conn *ct)
{
	if (nla_put_be32(skb, CTA_STATUS, htonl(ct->status)))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -1;
}