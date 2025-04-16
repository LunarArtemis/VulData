static int ctnetlink_dump_extinfo(struct sk_buff *skb,
				  struct nf_conn *ct, u32 type)
{
	if (ctnetlink_dump_acct(skb, ct, type) < 0 ||
	    ctnetlink_dump_timestamp(skb, ct) < 0 ||
	    ctnetlink_dump_helpinfo(skb, ct) < 0 ||
	    ctnetlink_dump_labels(skb, ct) < 0 ||
	    ctnetlink_dump_ct_seq_adj(skb, ct) < 0 ||
	    ctnetlink_dump_ct_synproxy(skb, ct) < 0)
		return -1;

	return 0;
}