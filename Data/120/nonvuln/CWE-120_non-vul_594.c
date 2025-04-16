static int
ctnetlink_glue_attach_expect(const struct nlattr *attr, struct nf_conn *ct,
			     u32 portid, u32 report)
{
	struct nlattr *cda[CTA_EXPECT_MAX+1];
	struct nf_conntrack_tuple tuple, mask;
	struct nf_conntrack_helper *helper = NULL;
	struct nf_conntrack_expect *exp;
	int err;

	err = nla_parse_nested_deprecated(cda, CTA_EXPECT_MAX, attr,
					  exp_nla_policy, NULL);
	if (err < 0)
		return err;

	err = ctnetlink_glue_exp_parse((const struct nlattr * const *)cda,
				       ct, &tuple, &mask);
	if (err < 0)
		return err;

	if (cda[CTA_EXPECT_HELP_NAME]) {
		const char *helpname = nla_data(cda[CTA_EXPECT_HELP_NAME]);

		helper = __nf_conntrack_helper_find(helpname, nf_ct_l3num(ct),
						    nf_ct_protonum(ct));
		if (helper == NULL)
			return -EOPNOTSUPP;
	}

	exp = ctnetlink_alloc_expect((const struct nlattr * const *)cda, ct,
				     helper, &tuple, &mask);
	if (IS_ERR(exp))
		return PTR_ERR(exp);

	err = nf_ct_expect_related_report(exp, portid, report, 0);
	nf_ct_expect_put(exp);
	return err;
}