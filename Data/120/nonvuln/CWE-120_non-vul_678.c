static int change_seq_adj(struct nf_ct_seqadj *seq,
			  const struct nlattr * const attr)
{
	int err;
	struct nlattr *cda[CTA_SEQADJ_MAX+1];

	err = nla_parse_nested_deprecated(cda, CTA_SEQADJ_MAX, attr,
					  seqadj_policy, NULL);
	if (err < 0)
		return err;

	if (!cda[CTA_SEQADJ_CORRECTION_POS])
		return -EINVAL;

	seq->correction_pos =
		ntohl(nla_get_be32(cda[CTA_SEQADJ_CORRECTION_POS]));

	if (!cda[CTA_SEQADJ_OFFSET_BEFORE])
		return -EINVAL;

	seq->offset_before =
		ntohl(nla_get_be32(cda[CTA_SEQADJ_OFFSET_BEFORE]));

	if (!cda[CTA_SEQADJ_OFFSET_AFTER])
		return -EINVAL;

	seq->offset_after =
		ntohl(nla_get_be32(cda[CTA_SEQADJ_OFFSET_AFTER]));

	return 0;
}