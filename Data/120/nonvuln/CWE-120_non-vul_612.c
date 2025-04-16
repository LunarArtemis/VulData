static int
ctnetlink_dump_acct(struct sk_buff *skb, const struct nf_conn *ct, int type)
{
	struct nf_conn_acct *acct = nf_conn_acct_find(ct);

	if (!acct)
		return 0;

	if (dump_counters(skb, acct, IP_CT_DIR_ORIGINAL, type) < 0)
		return -1;
	if (dump_counters(skb, acct, IP_CT_DIR_REPLY, type) < 0)
		return -1;

	return 0;
}