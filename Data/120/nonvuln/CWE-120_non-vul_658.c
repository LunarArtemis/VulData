static int ctnetlink_glue_exp_parse(const struct nlattr * const *cda,
				    const struct nf_conn *ct,
				    struct nf_conntrack_tuple *tuple,
				    struct nf_conntrack_tuple *mask)
{
	int err;

	err = ctnetlink_parse_tuple(cda, tuple, CTA_EXPECT_TUPLE,
				    nf_ct_l3num(ct), NULL);
	if (err < 0)
		return err;

	return ctnetlink_parse_tuple(cda, mask, CTA_EXPECT_MASK,
				     nf_ct_l3num(ct), NULL);
}