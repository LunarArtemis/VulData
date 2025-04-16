static int
ctnetlink_change_seq_adj(struct nf_conn *ct,
			 const struct nlattr * const cda[])
{
	struct nf_conn_seqadj *seqadj = nfct_seqadj(ct);
	int ret = 0;

	if (!seqadj)
		return 0;

	spin_lock_bh(&ct->lock);
	if (cda[CTA_SEQ_ADJ_ORIG]) {
		ret = change_seq_adj(&seqadj->seq[IP_CT_DIR_ORIGINAL],
				     cda[CTA_SEQ_ADJ_ORIG]);
		if (ret < 0)
			goto err;

		set_bit(IPS_SEQ_ADJUST_BIT, &ct->status);
	}

	if (cda[CTA_SEQ_ADJ_REPLY]) {
		ret = change_seq_adj(&seqadj->seq[IP_CT_DIR_REPLY],
				     cda[CTA_SEQ_ADJ_REPLY]);
		if (ret < 0)
			goto err;

		set_bit(IPS_SEQ_ADJUST_BIT, &ct->status);
	}

	spin_unlock_bh(&ct->lock);
	return 0;
err:
	spin_unlock_bh(&ct->lock);
	return ret;
}