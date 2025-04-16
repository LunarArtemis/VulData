static int ctnetlink_dump_timeout(struct sk_buff *skb, const struct nf_conn *ct)
{
	long timeout = nf_ct_expires(ct) / HZ;

	if (nla_put_be32(skb, CTA_TIMEOUT, htonl(timeout)))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -1;
}