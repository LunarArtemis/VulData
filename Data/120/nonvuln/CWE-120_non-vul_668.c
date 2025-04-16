static int
dump_ct_seq_adj(struct sk_buff *skb, const struct nf_ct_seqadj *seq, int type)
{
	struct nlattr *nest_parms;

	nest_parms = nla_nest_start(skb, type);
	if (!nest_parms)
		goto nla_put_failure;

	if (nla_put_be32(skb, CTA_SEQADJ_CORRECTION_POS,
			 htonl(seq->correction_pos)) ||
	    nla_put_be32(skb, CTA_SEQADJ_OFFSET_BEFORE,
			 htonl(seq->offset_before)) ||
	    nla_put_be32(skb, CTA_SEQADJ_OFFSET_AFTER,
			 htonl(seq->offset_after)))
		goto nla_put_failure;

	nla_nest_end(skb, nest_parms);

	return 0;

nla_put_failure:
	return -1;
}