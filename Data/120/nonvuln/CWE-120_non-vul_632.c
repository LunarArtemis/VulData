static void ctnetlink_glue_seqadj(struct sk_buff *skb, struct nf_conn *ct,
				  enum ip_conntrack_info ctinfo, int diff)
{
	if (!(ct->status & IPS_NAT_MASK))
		return;

	nf_ct_tcp_seqadj_set(skb, ct, ctinfo, diff);
}