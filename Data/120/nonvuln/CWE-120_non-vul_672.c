static int ctnetlink_dump_ct_seq_adj(struct sk_buff *skb, struct nf_conn *ct)
{
	struct nf_conn_seqadj *seqadj = nfct_seqadj(ct);
	struct nf_ct_seqadj *seq;

	if (!(ct->status & IPS_SEQ_ADJUST) || !seqadj)
		return 0;

	spin_lock_bh(&ct->lock);
	seq = &seqadj->seq[IP_CT_DIR_ORIGINAL];
	if (dump_ct_seq_adj(skb, seq, CTA_SEQ_ADJ_ORIG) == -1)
		goto err;

	seq = &seqadj->seq[IP_CT_DIR_REPLY];
	if (dump_ct_seq_adj(skb, seq, CTA_SEQ_ADJ_REPLY) == -1)
		goto err;

	spin_unlock_bh(&ct->lock);
	return 0;
err:
	spin_unlock_bh(&ct->lock);
	return -1;
}