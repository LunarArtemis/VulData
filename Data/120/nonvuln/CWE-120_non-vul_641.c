static struct nf_conntrack_expect *
ctnetlink_alloc_expect(const struct nlattr * const cda[], struct nf_conn *ct,
		       struct nf_conntrack_helper *helper,
		       struct nf_conntrack_tuple *tuple,
		       struct nf_conntrack_tuple *mask)
{
	u_int32_t class = 0;
	struct nf_conntrack_expect *exp;
	struct nf_conn_help *help;
	int err;

	help = nfct_help(ct);
	if (!help)
		return ERR_PTR(-EOPNOTSUPP);

	if (cda[CTA_EXPECT_CLASS] && helper) {
		class = ntohl(nla_get_be32(cda[CTA_EXPECT_CLASS]));
		if (class > helper->expect_class_max)
			return ERR_PTR(-EINVAL);
	}
	exp = nf_ct_expect_alloc(ct);
	if (!exp)
		return ERR_PTR(-ENOMEM);

	if (cda[CTA_EXPECT_FLAGS]) {
		exp->flags = ntohl(nla_get_be32(cda[CTA_EXPECT_FLAGS]));
		exp->flags &= ~NF_CT_EXPECT_USERSPACE;
	} else {
		exp->flags = 0;
	}
	if (cda[CTA_EXPECT_FN]) {
		const char *name = nla_data(cda[CTA_EXPECT_FN]);
		struct nf_ct_helper_expectfn *expfn;

		expfn = nf_ct_helper_expectfn_find_by_name(name);
		if (expfn == NULL) {
			err = -EINVAL;
			goto err_out;
		}
		exp->expectfn = expfn->expectfn;
	} else
		exp->expectfn = NULL;

	exp->class = class;
	exp->master = ct;
	exp->helper = helper;
	exp->tuple = *tuple;
	exp->mask.src.u3 = mask->src.u3;
	exp->mask.src.u.all = mask->src.u.all;

	if (cda[CTA_EXPECT_NAT]) {
		err = ctnetlink_parse_expect_nat(cda[CTA_EXPECT_NAT],
						 exp, nf_ct_l3num(ct));
		if (err < 0)
			goto err_out;
	}
	return exp;
err_out:
	nf_ct_expect_put(exp);
	return ERR_PTR(err);
}