static int ctnetlink_dump_id(struct sk_buff *skb, const struct nf_conn *ct)
{
	__be32 id = (__force __be32)nf_ct_get_id(ct);

	if (nla_put_be32(skb, CTA_ID, id))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -1;
}